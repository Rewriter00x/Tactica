#include "TacticaCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "Tactica/Components/WeaponComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ATacticaCharacter::ATacticaCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetCharacterMovement()->bOrientRotationToMovement = true;	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonCamera->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh"));
	HandsMesh->SetupAttachment(FirstPersonCamera);
	HandsMesh->SetOnlyOwnerSee(true);
	HandsMesh->bCastDynamicShadow = false;
	HandsMesh->CastShadow = false;
	HandsMesh->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	HandsMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FPSWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSWeaponMesh"));
	FPSWeaponMesh->SetupAttachment(HandsMesh, FPSAttachPointName);
	FPSWeaponMesh->SetOnlyOwnerSee(true);
	FPSWeaponMesh->bCastDynamicShadow = false;
	FPSWeaponMesh->CastShadow = false;
	FPSWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetOwnerNoSee(true);

	SetCanBeDamaged(true);
}

void ATacticaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, Health, COND_OwnerOnly);
	DOREPLIFETIME(ThisClass, SelectedWeapon);
}

void ATacticaCharacter::Destroyed()
{
	OnHealthChanged.Broadcast(0.f, Health);
	
	Super::Destroyed();
}

bool ATacticaCharacter::Server_BeginFire_Validate(UWeaponComponent* Weapon)
{
	return IsValid(Weapon);
}

void ATacticaCharacter::Server_BeginFire_Implementation(UWeaponComponent* Weapon)
{
	Weapon->Shoot();
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("SERVER BEGIN FIRE!!!"));
}

bool ATacticaCharacter::Server_EndFire_Validate(UWeaponComponent* Weapon)
{
	return IsValid(Weapon);
}

void ATacticaCharacter::Server_EndFire_Implementation(UWeaponComponent* Weapon)
{
	Weapon->StopAutoFire();
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("SERVER END FIRE!!!"));
}

bool ATacticaCharacter::Server_Reload_Validate(UWeaponComponent* Weapon)
{
	return IsValid(Weapon) && Weapon->CanReload();
}

void ATacticaCharacter::Server_Reload_Implementation(UWeaponComponent* Weapon)
{
	Weapon->PerformReload();
}

void ATacticaCharacter::SetFPSWeaponMesh(const USkeletalMeshComponent* Weapon) const
{
	FPSWeaponMesh->SetSkeletalMesh(Weapon->GetSkeletalMeshAsset());
	FPSWeaponMesh->SetMaterial(0, Weapon->GetMaterial(0));
}

void ATacticaCharacter::SetSelectedWeapon(UWeaponComponent* Weapon)
{
	SelectedWeapon = Weapon;
	OnSelectedWeaponChanged.Broadcast(SelectedWeapon);
}

FVector ATacticaCharacter::GetEyesLocation() const
{
	return FirstPersonCamera->GetComponentLocation();
}

FVector ATacticaCharacter::GetLookAtDirection() const
{
	return GetBaseAimRotation().Vector();
}

void ATacticaCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ATacticaCharacter::AnyDamageTaken);
	}
}

void ATacticaCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		OnTakeAnyDamage.RemoveDynamic(this, &ATacticaCharacter::AnyDamageTaken);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ATacticaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATacticaCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATacticaCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATacticaCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATacticaCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATacticaCharacter::AnyDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	check(this == DamagedActor);

	const float OldHealth = Health;
	Health -= Damage;
	OnHealthChanged.Broadcast(Health, OldHealth);
	if (Health <= 0.f)
	{
		Destroy();
	}
}

void ATacticaCharacter::OnRep_Health(float OldValue)
{
	OnHealthChanged.Broadcast(Health, OldValue);
}

void ATacticaCharacter::OnRep_SelectedWeapon(UWeaponComponent* OldValue)
{
	OnSelectedWeaponChanged.Broadcast(SelectedWeapon);
}
