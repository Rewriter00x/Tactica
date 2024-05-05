#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TacticaCharacter.generated.h"

class AWeapon;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSelectedWeaponChanged, AWeapon*);

UCLASS(Abstract, config=Game)
class ATacticaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATacticaCharacter();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AWeapon* GetSelectedWeapon() const { return SelectedWeapon; }
	
	FORCEINLINE UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }
	FORCEINLINE USkeletalMeshComponent* GetHandsMesh() const { return HandsMesh; }
	FORCEINLINE float GetHealth() const { return Health; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Destroyed() override;

	void SetFPSWeaponMesh(const USkeletalMeshComponent* Weapon) const;
	void SetSelectedWeapon(AWeapon* Weapon);

	FVector GetEyesLocation() const;
	FVector GetLookAtDirection() const;

	FOnHealthChanged OnHealthChanged;
	FOnSelectedWeaponChanged OnSelectedWeaponChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Health)
	float Health = 100.f;

private:
	UFUNCTION()
	void AnyDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnRep_Health(float OldValue);

	UFUNCTION()
	void OnRep_SelectedWeapon(AWeapon* OldValue);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attach, meta=(AllowPrivateAccess = "true"))
	FName FPSAttachPointName = TEXT("GripPoint");
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HandsMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FPSWeaponMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(ReplicatedUsing=OnRep_SelectedWeapon, Transient)
	AWeapon* SelectedWeapon = nullptr;

};

