#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TacticaHUD.generated.h"

class UScoreBoardWidget;

UCLASS(Abstract)
class TACTICA_API ATacticaHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> MainWidgetClass;

	UPROPERTY(BlueprintReadOnly, Transient)
	UUserWidget* MainWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> ScoreBoardWidgetClass;

	UPROPERTY(BlueprintReadOnly, Transient)
	UScoreBoardWidget* ScoreBoardWidget;
	
};
