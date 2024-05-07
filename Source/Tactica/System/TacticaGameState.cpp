#include "TacticaGameState.h"

#include "Net/UnrealNetwork.h"

void ATacticaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, WinningScore, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, MinNumberOfPlayers, COND_InitialOnly);
	DOREPLIFETIME(ThisClass, GameStatus);
	DOREPLIFETIME(ThisClass, NumberOfPlayers);
}

void ATacticaGameState::Multicast_BroadcastWinner_Implementation(ATacticaPlayerState* PlayerState)
{
	WinnerPlayerState = PlayerState;
	SetGameStatus(EGameStatus::Final);
}

void ATacticaGameState::SetWinningScore(const int32 Score)
{
	WinningScore = Score;
	OnWinningScoreChanged.Broadcast(WinningScore);
}

void ATacticaGameState::SetGameStatus(const EGameStatus Status)
{
	GameStatus = Status;
	OnGameStatusChanged.Broadcast(GameStatus);
}

void ATacticaGameState::SetMinNumberOfPlayers(const int32 Number)
{
	MinNumberOfPlayers = Number;
	OnMinNumberOfPlayersChanged.Broadcast(MinNumberOfPlayers);
}

void ATacticaGameState::SetNumberOfPlayers(const int32 Number)
{
	NumberOfPlayers = Number;
	OnNumberOfPlayersChanged.Broadcast(NumberOfPlayers);
}

void ATacticaGameState::OnRep_WinningScore()
{
	OnWinningScoreChanged.Broadcast(WinningScore);
}

void ATacticaGameState::OnRep_MinNumberOfPlayers()
{
	OnMinNumberOfPlayersChanged.Broadcast(MinNumberOfPlayers);
}

void ATacticaGameState::OnRep_NumberOfPlayers()
{
	OnNumberOfPlayersChanged.Broadcast(NumberOfPlayers);
}

void ATacticaGameState::OnRep_GameStatus()
{
	OnGameStatusChanged.Broadcast(GameStatus);
}
