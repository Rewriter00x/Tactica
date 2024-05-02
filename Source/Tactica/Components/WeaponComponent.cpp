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
		Rename(*GetFName().ToString(), TargetCharacter);
	}

	const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(TargetCharacter->GetMesh(), AttachmentRules, TPSAttachPointName);
	
	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TargetCharacter->AddInstanceComponent(this);

	// TODO add mesh to tps

	if (const APlayerController* PlayerController = Cast<APlayerController>(TargetCharacter->GetController()))
	{
		if (!PlayerController->IsLocalController())
		{
			return;
		}
		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &UWeaponComponent::BeginFire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &UWeaponComponent::EndFire);
		}
	}
}

void UWeaponComponent::BeginFire()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("BEGIN FIRE!!!"));
}

void UWeaponComponent::EndFire()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("END FIRE!!!"));
}
