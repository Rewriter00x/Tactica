#include "GameStatusWidget.h"

#include "Components/TextBlock.h"
#include "Tactica/Character/TacticaPlayerState.h"
#include "Tactica/System/TacticaGameState.h"

void UGameStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ATacticaGameState* GameState = GetWorld()->GetGameState<ATacticaGameState>())
	{
		TacticaGameState = GameState;
		StatusDelegateHandle = TacticaGameState->OnGameStatusChanged.AddUObject(this, &ThisClass::OnGameStatusChanged);
		NumberDelegateHandle = TacticaGameState->OnNumberOfPlayersChanged.AddUObject(this, &ThisClass::OnNumberOfPlayersChanged);
		OnGameStatusChanged(TacticaGameState->GetGameStatus());
	}
}

void UGameStatusWidget::NativeDestruct()
{
	if (TacticaGameState)
	{
		TacticaGameState->OnGameStatusChanged.Remove(StatusDelegateHandle);
		TacticaGameState->OnNumberOfPlayersChanged.Remove(NumberDelegateHandle);
	}
	
	Super::NativeDestruct();
}

void UGameStatusWidget::OnGameStatusChanged(EGameStatus Status) const
{
	FString Message;
	
	switch (Status)
	{
	case EGameStatus::Training:
		Message = TEXT("Practice your Skills!");
		break;
	case EGameStatus::WaitingForPlayers:
		Message = FString::Printf(TEXT("Waiting for players: %d/%d"), TacticaGameState->GetNumberOfPlayers(), TacticaGameState->GetMinNumberOfPlayers());
		break;
	case EGameStatus::InProgress:
		Message = FString::Printf(TEXT("Game Started! Score %d points to win!"), TacticaGameState->GetWinningScore());
		break;
	case EGameStatus::Final:
		Message = FString::Printf(TEXT("Game over! %s won!"), *TacticaGameState->GetWinnerPlayerState()->GetPlayerName());
		break;
	default:
		Message = TEXT("Bad Status!");
		break;
	}
	
	StatusText->SetText(FText::FromString(Message));
}

void UGameStatusWidget::OnNumberOfPlayersChanged(int32 Number) const
{
	StatusText->SetText(FText::FromString(FString::Printf(TEXT("Waiting for players: %d/%d"), Number, TacticaGameState->GetMinNumberOfPlayers())));
}
