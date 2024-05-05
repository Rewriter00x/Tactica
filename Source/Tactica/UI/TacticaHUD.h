#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TacticaHUD.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UScoreBoardWidget;

UCLASS(Abstract)
class TACTICA_API ATacticaHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> MainWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> ScoreBoardWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputMappingContext* ShowMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* ShowAction;

	UPROPERTY(BlueprintReadOnly, Transient)
	UUserWidget* MainWidget;

	UPROPERTY(BlueprintReadOnly, Transient)
	UScoreBoardWidget* ScoreBoardWidget;

private:
	void ChangeScoreBoardVisibility(const FInputActionValue& Value);
	
};
