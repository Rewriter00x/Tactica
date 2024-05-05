#include "ScoreBoardWidget.h"

#include "ScoreCellWidget.h"
#include "Components/VerticalBox.h"
#include "Tactica/Character/TacticaPlayerState.h"
#include "Tactica/System/TacticaGameState.h"

void UScoreBoardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ATacticaGameState* GameState = Cast<ATacticaGameState>(GetWorld()->GetGameState()))
	{
		for (APlayerState* PlayerState : GameState->PlayerArray)
		{
			if (ATacticaPlayerState* TacticaPlayerState = Cast<ATacticaPlayerState>(PlayerState))
			{
				OnPlayerStateSpawned(TacticaPlayerState);
			}
		}

		PlayerStateSpawnedHandle = GameState->OnPlayerStateSpawned.AddUObject(this, &UScoreBoardWidget::OnPlayerStateSpawned);
		PlayerStateDeSpawnedHandle = GameState->OnPlayerStateDeSpawned.AddUObject(this, &UScoreBoardWidget::OnPlayerStateDeSpawned);
	}
}

void UScoreBoardWidget::NativeDestruct()
{
	if (ATacticaGameState* GameState = Cast<ATacticaGameState>(GetWorld()->GetGameState()))
	{
		GameState->OnPlayerStateSpawned.Remove(PlayerStateSpawnedHandle);
		GameState->OnPlayerStateDeSpawned.Remove(PlayerStateDeSpawnedHandle);
	}
	
	Super::NativeDestruct();
}

void UScoreBoardWidget::OnPlayerStateSpawned(ATacticaPlayerState* PlayerState) const
{
	UScoreCellWidget* ScoreCell = CreateWidget<UScoreCellWidget>(GetOwningPlayer(), ScoreCellClass);
	ScoreCell->InitCell(PlayerState);
	ScoreBox->AddChildToVerticalBox(ScoreCell);
}

void UScoreBoardWidget::OnPlayerStateDeSpawned(ATacticaPlayerState* PlayerState) const
{
	const TArray<UWidget*> Children = ScoreBox->GetAllChildren();
	for (int32 i = 0; i < Children.Num(); ++i)
	{
		if (const UScoreCellWidget* ScoreCell = Cast<UScoreCellWidget>(Children[i]))
		{
			if (ScoreCell->GetObservingPlayerState() == PlayerState)
			{
				ScoreBox->RemoveChildAt(i);
			}
		}
	}
}
