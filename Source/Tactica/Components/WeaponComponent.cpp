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

void UWeaponComponent::BeginFire()
{
	if (OwningCharacter)
	{
		OwningCharacter->Server_BeginFire(this);
	}
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
