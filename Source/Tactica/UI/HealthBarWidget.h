#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UProgressBar;

UCLASS(Abstract)
class TACTICA_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UProgressBar* HealthProgressBar;

private:
	UFUNCTION()
	void BindPawn(APawn* OldPawn, APawn* NewPawn);

	void OnPossessedPawnHealthChanged(float NewValue, float OldValue) const;

	FDelegateHandle DelegateHandle;
	
};
