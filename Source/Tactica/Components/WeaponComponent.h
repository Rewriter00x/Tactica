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

	void TraceForTarget();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void AttachWeapon(ATacticaCharacter* TargetCharacter);

private:
	void BeginFire();
	void EndFire();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attach, meta=(AllowPrivateAccess = "true"))
	FName TPSAttachPointName = TEXT("weapon_r_socket");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "90"))
	float Spread = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess = "true"))
	bool bIsAutomatic = false;
	
	UPROPERTY(Replicated)
	ATacticaCharacter* OwningCharacter;
	
};
