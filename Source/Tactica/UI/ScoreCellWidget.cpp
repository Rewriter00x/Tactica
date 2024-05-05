#include "ScoreCellWidget.h"

#include "Components/TextBlock.h"
#include "Tactica/Character/TacticaPlayerState.h"

void UScoreCellWidget::SetObservingPlayerState(ATacticaPlayerState* PlayerState)
{
	ObservingPlayerState = PlayerState;
}

void UScoreCellWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ObservingPlayerState)
	{
		DelegateHandle = ObservingPlayerState->OnPlayerScoreChanged.AddUObject(this, &UScoreCellWidget::OnPlayerScoreChanged);
		OnPlayerScoreChanged(ObservingPlayerState, ObservingPlayerState->GetPlayerScore());

		if (const AController* Controller = ObservingPlayerState->GetOwningController())
		{
			if (Controller->IsLocalPlayerController())
			{
				NameText->SetColorAndOpacity(FColor::Green);
				ScoreText->SetColorAndOpacity(FColor::Green);
			}
		}
	}
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
