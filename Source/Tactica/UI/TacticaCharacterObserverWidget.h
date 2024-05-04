#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TacticaCharacterObserverWidget.generated.h"

UCLASS(Abstract)
class TACTICA_API UTacticaCharacterObserverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	virtual void BindPawn(APawn* OldPawn, APawn* NewPawn);
	
};
