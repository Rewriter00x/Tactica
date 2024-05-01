#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TacticaHUD.generated.h"

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
	
};
