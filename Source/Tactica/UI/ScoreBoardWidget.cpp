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

		for (APlayerState* PlayerState : GameState->PlayerArray)
		{
			if (ATacticaPlayerState* TacticaPlayerState = Cast<ATacticaPlayerState>(PlayerState))
			{
				TacticaPlayerState->OnPlayerScoreChanged.RemoveAll(this);
			}
		}
	}
	
	Super::NativeDestruct();
}

void UScoreBoardWidget::OnPlayerStateSpawned(ATacticaPlayerState* PlayerState) const
{
	PlayerState->OnPlayerScoreChanged.AddUObject(this, &UScoreBoardWidget::OnPlayerScoreChanged);
	
	UScoreCellWidget* ScoreCell = CreateWidget<UScoreCellWidget>(GetOwningPlayer(), ScoreCellClass);
	ScoreCell->SetObservingPlayerState(PlayerState);
	ScoreBox->AddChildToVerticalBox(ScoreCell);
	SortScoreBoard();
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
				break;
			}
		}
	}
}

void UScoreBoardWidget::OnPlayerScoreChanged(ATacticaPlayerState* PlayerState, int32 Score) const
{
	SortScoreBoard();
}

void UScoreBoardWidget::SortScoreBoard() const
{
	TArray<UScoreCellWidget*> Cells;
	for (UWidget* Widget : ScoreBox->GetAllChildren())
	{
		if (UScoreCellWidget* Cell = Cast<UScoreCellWidget>(Widget))
		{
			Cells.Add(Cell);
		}
	}

	Cells.HeapSort([](const UScoreCellWidget& Left, const UScoreCellWidget& Right) // They really didn't want to take actor pointers, had to go with refs
	{
		const ATacticaPlayerState* LeftState = Left.GetObservingPlayerState();
		const ATacticaPlayerState* RightState = Right.GetObservingPlayerState();

		const int32 LeftScore = LeftState->GetPlayerScore();
		const int32 RightScore = RightState->GetPlayerScore();

		if (LeftScore != RightScore)
		{
			return LeftScore > RightScore;
		}
		
		return LeftState->GetPlayerName().Compare(RightState->GetPlayerName()) < 0;
	});

	for (UScoreCellWidget* Cell : Cells)
	{
		ScoreBox->AddChildToVerticalBox(Cell);
	}
}
