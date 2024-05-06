#pragma once

#include "CoreMinimal.h"
#include "TacticaCharacterObserverWidget.h"
#include "CrosshairWidget.generated.h"

class AWeapon;
class UImage;

UCLASS(Abstract)
class TACTICA_API UCrosshairWidget : public UTacticaCharacterObserverWidget
{
	GENERATED_BODY()

protected:
	virtual void BindPawn(APawn* OldPawn, APawn* NewPawn) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UImage* CrosshairImage;

private:
	void OnPossessedPawnSelectedWeaponChanged(AWeapon* NewWeapon) const;

	FDelegateHandle DelegateHandle;
	
};
