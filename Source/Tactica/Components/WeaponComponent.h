#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponComponent.generated.h"

class ATacticaCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TACTICA_API UWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BeginShot(const FVector& Start, const FVector& End);

	bool CheckLastShotTime() const;
	bool CheckAndChangeLastShotTime();
	
	void TraceForTarget();

	void Shoot();
	void StopAutoFire();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void AttachWeapon(ATacticaCharacter* TargetCharacter);

	void DrawLocalFire(const FVector& Start, const FVector& End) const;

private:
	void BeginFire();
	void EndFire();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attach, meta=(AllowPrivateAccess = "true"))
	FName TPSAttachPointName = TEXT("weapon_r_socket");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta = (AllowPrivateAccess = "true"))
	float DamagePerBullet = 15.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta = (AllowPrivateAccess = "true"))
	float BulletDistance = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta = (AllowPrivateAccess = "true"))
	float ShotDelay = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "90"))
	float Spread = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess = "true"))
	bool bIsAutomatic = false;
	
	UPROPERTY(Replicated)
	ATacticaCharacter* OwningCharacter;

	FTimerHandle ShotDelayHandle;
	
};
