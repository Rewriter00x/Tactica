#include "WeaponComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Tactica/Character/TacticaCharacter.h"

UWeaponComponent::UWeaponComponent()
{
	SetIsReplicatedByDefault(true);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, OwningCharacter);
	DOREPLIFETIME(ThisClass, LoadedAmmo);
	DOREPLIFETIME(ThisClass, SpareAmmo);
}

void UWeaponComponent::Multicast_BeginShot_Implementation(const FVector& Start, const FVector& End)
{
	DrawLocalFire(Start, End);
}

void UWeaponComponent::TraceForTarget()
{
	if (ensure(OwningCharacter))
	{
		const FVector StartLocation = OwningCharacter->GetEyesLocation();
		const FVector DirectionVector = FMath::VRandCone(
			OwningCharacter->GetLookAtDirection(),
			FMath::DegreesToRadians(Spread));
		const FVector EndLocation = StartLocation + DirectionVector * BulletDistance;

		FHitResult FiringResult;
		FCollisionQueryParams Params;
		Params.bTraceComplex = false;
		Params.AddIgnoredActor(OwningCharacter);
		Params.AddIgnoredComponent(this);
		GetWorld()->LineTraceSingleByChannel(FiringResult, StartLocation, EndLocation, ECC_Pawn, Params);

		const FVector& HitLocation = FiringResult.Location.IsNearlyZero() ? EndLocation : FiringResult.Location;

		Multicast_BeginShot(StartLocation, HitLocation);
		
		if (ATacticaCharacter* HitCharacter = Cast<ATacticaCharacter>(FiringResult.GetActor()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("%s hit!"), *HitCharacter->GetFName().ToString()));
			UGameplayStatics::ApplyDamage(HitCharacter, DamagePerBullet, nullptr, OwningCharacter, UDamageType::StaticClass());
		}
	}
}

void UWeaponComponent::Shoot()
{
	if (CheckAndCommitCost())
	{
		TraceForTarget();
		if (bIsAutomatic)
		{
			GetWorld()->GetTimerManager().SetTimer(ShotDelayHandle, this, &ThisClass::TryShoot, ShotDelay);
		}
	}
}

void UWeaponComponent::StopAutoFire()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	const float Delay = TimerManager.GetTimerRemaining(ShotDelayHandle);
	TimerManager.ClearTimer(ShotDelayHandle);
	TimerManager.SetTimer(ShotDelayHandle, Delay, false);
}

void UWeaponComponent::PerformReload()
{
	int32 Delta = MagazineSize - LoadedAmmo;
	if (SpareAmmo < Delta && SpareAmmo > 0)
	{
		Delta = SpareAmmo;
	}
	LoadedAmmo += Delta;
	SpareAmmo -= Delta;
	OnWeaponAmmoChanged.Broadcast(LoadedAmmo, SpareAmmo);
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadedAmmo = MagazineSize;
	SpareAmmo = MagazineSize * MagazineCount;
	OnWeaponAmmoChanged.Broadcast(LoadedAmmo, SpareAmmo);
}

bool UWeaponComponent::CheckCost() const
{
	return !GetWorld()->GetTimerManager().IsTimerActive(ShotDelayHandle) && LoadedAmmo > 0;
}

bool UWeaponComponent::CheckAndCommitCost()
{
	if (!CheckCost())
	{
		return false;
	}

	GetWorld()->GetTimerManager().SetTimer(ShotDelayHandle, ShotDelay, false);
	--LoadedAmmo;
	
	OnWeaponAmmoChanged.Broadcast(LoadedAmmo, SpareAmmo);
	return true;
}

bool UWeaponComponent::CanReload() const
{
	return LoadedAmmo < MagazineSize && SpareAmmo > 0;
}

void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!OwningCharacter)
	{
		return;
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		if (!PlayerController->IsLocalController())
		{
			return;
		}
		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void UWeaponComponent::AttachWeapon_Implementation(ATacticaCharacter* TargetCharacter)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		OwningCharacter = TargetCharacter;
		UnregisterComponent();
		Rename(*GetFName().ToString(), TargetCharacter);
		RegisterComponent();
		TargetCharacter->SetSelectedWeapon(this);
	}

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(TargetCharacter->GetMesh(), AttachmentRules, TPSAttachPointName);
	
	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TargetCharacter->AddInstanceComponent(this);

	if (const APlayerController* PlayerController = Cast<APlayerController>(TargetCharacter->GetController()))
	{
		if (!PlayerController->IsLocalController())
		{
			return;
		}

		TargetCharacter->SetFPSWeaponMesh(this);
		SetHiddenInGame(true);
		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &UWeaponComponent::BeginFire);
			if (bIsAutomatic)
			{
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &UWeaponComponent::EndFire);
			}
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &UWeaponComponent::Reload);
		}
	}
}

void UWeaponComponent::DrawLocalFire(const FVector& Start, const FVector& End) const
{
	bool Local = false;
	const AController* Controller = OwningCharacter ? OwningCharacter->GetController() : nullptr;
	if (Controller && Controller->IsLocalPlayerController())
	{
		Local = true;
	}
	DrawDebugLine(GetWorld(), Start, End, FColor(243, 156, 18), false, .1f, 0, Local ? .2f : .75f);
}

void UWeaponComponent::OnRep_LoadedAmmo(int32 OldValue)
{
	OnWeaponAmmoChanged.Broadcast(LoadedAmmo, SpareAmmo);
}

void UWeaponComponent::OnRep_SpareAmmo(int32 OldValue)
{
	OnWeaponAmmoChanged.Broadcast(LoadedAmmo, SpareAmmo);
}

void UWeaponComponent::BeginFire()
{
	if (!OwningCharacter)
	{
		return;
	}

	if (!OwningCharacter->HasAuthority())
	{
		if (!CheckAndCommitCost()) // client check
		{
			return;
		}
	}
	
	OwningCharacter->Server_BeginFire(this);
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("BEGIN FIRE!!!"));
}

void UWeaponComponent::EndFire()
{
	if (OwningCharacter)
	{
		OwningCharacter->Server_EndFire(this);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("END FIRE!!!"));
}

void UWeaponComponent::Reload()
{
	if (!OwningCharacter)
	{
		return;
	}
	
	if (!CanReload())
	{
		return;
	}
	
	OwningCharacter->Server_Reload(this);

	if (!OwningCharacter->HasAuthority())
	{
		PerformReload();
	}
}

void UWeaponComponent::TryShoot()
{
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]
	{
		if (CheckAndCommitCost())
		{
			TraceForTarget();
			GetWorld()->GetTimerManager().SetTimer(ShotDelayHandle, this, &ThisClass::TryShoot, ShotDelay);
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(ShotDelayHandle);
		}
	});
}
