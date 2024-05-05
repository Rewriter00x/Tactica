#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Tactica/Character/TacticaPlayerState.h"
#include "ScoreBoardWidget.generated.h"

class UScoreCellWidget;
class UVerticalBox;
class ATacticaPlayerState;

UCLASS(Abstract)
class TACTICA_API UScoreBoardWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	UVerticalBox* ScoreBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UScoreCellWidget> ScoreCellClass;

private:
	void OnPlayerStateSpawned(ATacticaPlayerState* PlayerState) const;
	void OnPlayerStateDeSpawned(ATacticaPlayerState* PlayerState) const;

	FDelegateHandle PlayerStateSpawnedHandle;
	FDelegateHandle PlayerStateDeSpawnedHandle;
	
};
