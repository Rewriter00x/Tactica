#include "Weapon.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Tactica/Character/TacticaCharacter.h"

AWeapon::AWeapon()
{
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(WeaponMesh);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, OwningCharacter);
	DOREPLIFETIME(ThisClass, LoadedAmmo);
	DOREPLIFETIME(ThisClass, SpareAmmo);
}

void AWeapon::Multicast_BeginShot_Implementation(const FVector& Start, const FVector& End)
{
	DrawLocalFire(Start, End);
}

void AWeapon::TraceForTarget()
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
		Params.AddIgnoredActor(this);
		GetWorld()->LineTraceSingleByChannel(FiringResult, StartLocation, EndLocation, ECC_Pawn, Params);

		const FVector& HitLocation = FiringResult.Location.IsNearlyZero() ? EndLocation : FiringResult.Location;

		Multicast_BeginShot(StartLocation, HitLocation);
		
		if (ATacticaCharacter* HitCharacter = Cast<ATacticaCharacter>(FiringResult.GetActor()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("%s hit!"), *HitCharacter->GetFName().ToString()));
			UGameplayStatics::ApplyDamage(HitCharacter, DamagePerBullet, OwningCharacter->GetController(), OwningCharacter, UDamageType::StaticClass());
		}
	}
}

void AWeapon::Shoot()
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

void AWeapon::StopAutoFire()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	const float Delay = TimerManager.GetTimerRemaining(ShotDelayHandle);
	TimerManager.ClearTimer(ShotDelayHandle);
	TimerManager.SetTimer(ShotDelayHandle, Delay, false);
}

void AWeapon::PerformReload()
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

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	LoadedAmmo = MagazineSize;
	SpareAmmo = MagazineSize * MagazineCount;
	OnWeaponAmmoChanged.Broadcast(LoadedAmmo, SpareAmmo);
}

bool AWeapon::CheckCost() const
{
	return !GetWorld()->GetTimerManager().IsTimerActive(ShotDelayHandle) && LoadedAmmo > 0;
}

bool AWeapon::CheckAndCommitCost()
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

bool AWeapon::CanReload() const
{
	return LoadedAmmo < MagazineSize && SpareAmmo > 0;
}

void AWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

void AWeapon::AttachWeapon_Implementation(ATacticaCharacter* TargetCharacter)
{
	if (HasAuthority())
	{
		OwningCharacter = TargetCharacter;
		SetOwner(TargetCharacter);
		SetInstigator(TargetCharacter);
		TargetCharacter->SetSelectedWeapon(this);
	}

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(TargetCharacter->GetMesh(), AttachmentRules, TPSAttachPointName);

	if (const APlayerController* PlayerController = Cast<APlayerController>(TargetCharacter->GetController()))
	{
		if (!PlayerController->IsLocalController())
		{
			return;
		}

		TargetCharacter->SetFPSWeaponMesh(WeaponMesh);
		WeaponMesh->SetHiddenInGame(true);
		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::BeginFire);
			if (bIsAutomatic)
			{
				EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ThisClass::EndFire);
			}
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ThisClass::Reload);
		}
	}
}

void AWeapon::DrawLocalFire(const FVector& Start, const FVector& End) const
{
	bool Local = false;
	const AController* Controller = OwningCharacter ? OwningCharacter->GetController() : nullptr;
	if (Controller && Controller->IsLocalPlayerController())
	{
		Local = true;
	}
	DrawDebugLine(GetWorld(), Start, End, FColor(243, 156, 18), false, .1f, 0, Local ? .2f : .75f);
}

bool AWeapon::Server_BeginFire_Validate()
{
	return IsValid(this);
}

void AWeapon::Server_BeginFire_Implementation()
{
	Shoot();
}

bool AWeapon::Server_EndFire_Validate()
{
	return IsValid(this);
}

void AWeapon::Server_EndFire_Implementation()
{
	StopAutoFire();
}

bool AWeapon::Server_Reload_Validate()
{
	return IsValid(this) && CanReload();
}

void AWeapon::Server_Reload_Implementation()
{
	PerformReload();
}

void AWeapon::OnRep_LoadedAmmo(int32 OldValue)
{
	OnWeaponAmmoChanged.Broadcast(LoadedAmmo, SpareAmmo);
}

void AWeapon::OnRep_SpareAmmo(int32 OldValue)
{
	OnWeaponAmmoChanged.Broadcast(LoadedAmmo, SpareAmmo);
}

void AWeapon::BeginFire()
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
	
	Server_BeginFire();
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("BEGIN FIRE!!!"));
}

void AWeapon::EndFire()
{
	Server_EndFire();
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("END FIRE!!!"));
}

void AWeapon::Reload()
{
	if (!OwningCharacter)
	{
		return;
	}
	
	if (!CanReload())
	{
		return;
	}
	
	Server_Reload();

	if (!OwningCharacter->HasAuthority())
	{
		PerformReload();
	}
}

void AWeapon::TryShoot()
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
