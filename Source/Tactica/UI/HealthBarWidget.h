#pragma once

#include "CoreMinimal.h"
#include "TacticaCharacterObserverWidget.h"
#include "HealthBarWidget.generated.h"

class UProgressBar;

UCLASS(Abstract)
class TACTICA_API UHealthBarWidget : public UTacticaCharacterObserverWidget
{
	GENERATED_BODY()

protected:
	virtual void BindPawn(APawn* OldPawn, APawn* NewPawn) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UProgressBar* HealthProgressBar;

private:
	void OnPossessedPawnHealthChanged(float NewValue, float OldValue) const;

	FDelegateHandle DelegateHandle;
	
};
