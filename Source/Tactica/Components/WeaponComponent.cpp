#include "WeaponComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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

void UWeaponComponent::TraceForTarget()
{
	if (ensure(OwningCharacter))
	{
		constexpr float BulletPathLength = 275'000.f;
		
		const FVector StartLocation = OwningCharacter->GetEyesLocation();
		const FVector DirectionVector = FMath::VRandCone(
			OwningCharacter->GetLookAtDirection(),
			FMath::DegreesToRadians(Spread));
		const FVector EndLocation = StartLocation + DirectionVector * BulletPathLength;

		FHitResult FiringResult;
		FCollisionQueryParams Params;
		Params.bTraceComplex = false;
		Params.AddIgnoredActor(OwningCharacter);
		Params.AddIgnoredComponent(this);
		GetWorld()->LineTraceSingleByChannel(FiringResult, StartLocation, EndLocation, ECC_Visibility, Params);

		DrawDebugLine(GetWorld(), StartLocation, FiringResult.Location, FColor(243, 156, 18), false, .1f, 0, .2f);
		
		if (AActor* HitActor = FiringResult.GetActor())
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("%s Hit!"), *HitActor->GetFName().ToString()));
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

void UWeaponComponent::BeginFire()
{
	if (OwningCharacter)
	{
		OwningCharacter->Server_BeginFire(this);
	}
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("BEGIN FIRE!!!"));
}

void UWeaponComponent::EndFire()
{
	if (OwningCharacter)
	{
		OwningCharacter->Server_EndFire(this);
	}
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("END FIRE!!!"));
}
