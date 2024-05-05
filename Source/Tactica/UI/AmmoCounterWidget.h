#pragma once

#include "CoreMinimal.h"
#include "TacticaCharacterObserverWidget.h"
#include "AmmoCounterWidget.generated.h"

class AWeapon;
class UTextBlock;

UCLASS(Abstract)
class TACTICA_API UAmmoCounterWidget : public UTacticaCharacterObserverWidget
{
	GENERATED_BODY()

protected:
	virtual void BindPawn(APawn* OldPawn, APawn* NewPawn) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* AmmoTextBlock;

private:
	void OnSelectedWeaponChanged(AWeapon* Weapon);
	void OnSelectedWeaponAmmoChanged(int32 LoadedAmmo, int32 SpareAmmo) const;

	UPROPERTY(Transient)
	AWeapon* SelectedWeapon = nullptr;

	FDelegateHandle WeaponDelegateHandle;
	FDelegateHandle AmmoDelegateHandle;
	
};
