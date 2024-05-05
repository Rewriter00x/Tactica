#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class ATacticaCharacter;
class UInputAction;
class UInputMappingContext;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWeaponAmmoChanged, int32, int32);

UCLASS(Abstract)
class TACTICA_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RefillAmmo();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BeginShot(const FVector& Start, const FVector& End);

	FORCEINLINE int32 GetLoadedAmmo() const { return LoadedAmmo; }
	FORCEINLINE int32 GetSpareAmmo() const { return SpareAmmo; }

	bool CheckCost() const;
	bool CheckAndCommitCost();
	bool CanReload() const;
	
	void TraceForTarget();

	void Shoot();
	void StopAutoFire();
	void PerformReload();

	FOnWeaponAmmoChanged OnWeaponAmmoChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void AttachWeapon(ATacticaCharacter* TargetCharacter);

	void DrawLocalFire(const FVector& Start, const FVector& End) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* WeaponMesh;

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_BeginFire();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EndFire();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reload();
	
	UFUNCTION()
	void OnRep_LoadedAmmo(int32 OldValue);

	UFUNCTION()
	void OnRep_SpareAmmo(int32 OldValue);
	
	void BeginFire();
	void EndFire();
	void Reload();

	void TryShoot();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attach, meta=(AllowPrivateAccess = "true"))
	FName TPSAttachPointName = TEXT("weapon_r_socket");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* FireMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FireAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta = (AllowPrivateAccess = "true"))
	float DamagePerBullet = 15.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta = (AllowPrivateAccess = "true"))
	float BulletDistance = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta = (AllowPrivateAccess = "true"))
	int32 MagazineSize = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta = (AllowPrivateAccess = "true"))
	int32 MagazineCount = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta = (AllowPrivateAccess = "true"))
	float ShotDelay = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "90"))
	float Spread = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess = "true"))
	bool bIsAutomatic = false;
	
	UPROPERTY(Replicated)
	ATacticaCharacter* OwningCharacter;

	UPROPERTY(ReplicatedUsing=OnRep_LoadedAmmo)
	int32 LoadedAmmo;

	UPROPERTY(ReplicatedUsing=OnRep_SpareAmmo)
	int32 SpareAmmo;

	FTimerHandle ShotDelayHandle;
	
};
