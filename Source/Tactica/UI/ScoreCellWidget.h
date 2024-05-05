#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreCellWidget.generated.h"

class ATacticaPlayerState;
class UTextBlock;

UCLASS(Abstract)
class TACTICA_API UScoreCellWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE ATacticaPlayerState* GetObservingPlayerState() const { return ObservingPlayerState; }
	
	void SetObservingPlayerState(ATacticaPlayerState* PlayerState);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* NameText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* ScoreText;

private:
	void OnPlayerScoreChanged(ATacticaPlayerState* PlayerState, int32 Score) const;
	
	UPROPERTY(Transient)
	ATacticaPlayerState* ObservingPlayerState;

	FDelegateHandle DelegateHandle;
	
};
