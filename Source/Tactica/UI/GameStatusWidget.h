#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Tactica/System/GameStatus.h"
#include "GameStatusWidget.generated.h"

class ATacticaGameState;
class UTextBlock;

UCLASS(Abstract)
class TACTICA_API UGameStatusWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* StatusText;

	UPROPERTY(Transient)
	ATacticaGameState* TacticaGameState;

private:
	void OnGameStatusChanged(EGameStatus Status) const;
	void OnNumberOfPlayersChanged(int32 Number) const;

	FDelegateHandle StatusDelegateHandle;
	FDelegateHandle NumberDelegateHandle;
	
};
