#include "ScoreCellWidget.h"

#include "Components/TextBlock.h"
#include "Tactica/Character/TacticaPlayerState.h"

void UScoreCellWidget::InitCell(ATacticaPlayerState* PlayerState)
{
	ObservingPlayerState = PlayerState;
	DelegateHandle = ObservingPlayerState->OnPlayerScoreChanged.AddUObject(this, &UScoreCellWidget::OnPlayerScoreChanged);
	OnPlayerScoreChanged(ObservingPlayerState, ObservingPlayerState->GetScore());
}

void UScoreCellWidget::NativeDestruct()
{
	if (ObservingPlayerState)
	{
		ObservingPlayerState->OnPlayerScoreChanged.Remove(DelegateHandle);
	}
	
	Super::NativeDestruct();
}

void UScoreCellWidget::OnPlayerScoreChanged(ATacticaPlayerState* PlayerState, int32 Score) const
{
	NameText->SetText(FText::FromString(PlayerState->GetPlayerName()));
	ScoreText->SetText(FText::FromString(FString::FromInt(Score)));
}
