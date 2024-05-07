#include "TacticaGameMode.h"

#include "TacticaGameState.h"
#include "Tactica/Character/TacticaPlayerState.h"

void ATacticaGameMode::PostLogin(APlayerController* NewPlayer)
{
	if (GetNetMode() == NM_Standalone)
	{
		Super::PostLogin(NewPlayer);
		return;
	}
	
	if (!TacticaGameState)
	{
		FindGameState();
	}
	
	++NumberOfPlayers;
	TacticaGameState->SetNumberOfPlayers(NumberOfPlayers);
	
	if (NumberOfPlayers >= MinNumberOfPlayers && TacticaGameState->GetGameStatus() == EGameStatus::WaitingForPlayers)
	{
		for (APlayerState* PlayerState : TacticaGameState->PlayerArray)
		{
			if (ATacticaPlayerState* TacticaPlayerState = Cast<ATacticaPlayerState>(PlayerState))
			{
				TacticaPlayerState->ClearPlayerScore();
			}
		}
		TacticaGameState->SetGameStatus(EGameStatus::InProgress);
	}
	
	Super::PostLogin(NewPlayer);
}

void ATacticaGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!TacticaGameState)
	{
		FindGameState();
	}

	if (GetNetMode() == NM_Standalone)
	{
		TacticaGameState->SetGameStatus(EGameStatus::Training);
		return;
	}

	TacticaGameState->SetGameStatus(EGameStatus::WaitingForPlayers);
	TacticaGameState->SetWinningScore(WinningScore);
	TacticaGameState->SetMinNumberOfPlayers(MinNumberOfPlayers);
	TacticaGameState->SetNumberOfPlayers(NumberOfPlayers);
	
	for (APlayerState* PlayerState : TacticaGameState->PlayerArray)
	{
		if (ATacticaPlayerState* TacticaPlayerState = Cast<ATacticaPlayerState>(PlayerState))
		{
			OnPlayerStateSpawned(TacticaPlayerState);
		}
	}

	PlayerStateSpawnedHandle = TacticaGameState->OnPlayerStateSpawned.AddUObject(this, &ATacticaGameMode::OnPlayerStateSpawned);
}

void ATacticaGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TacticaGameState)
	{
		TacticaGameState->OnPlayerStateSpawned.Remove(PlayerStateSpawnedHandle);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ATacticaGameMode::OnPlayerStateSpawned(ATacticaPlayerState* PlayerState) const
{
	PlayerState->OnPlayerScoreChanged.AddUObject(this, &ATacticaGameMode::OnPlayerStateScoreChanged);
}

void ATacticaGameMode::OnPlayerStateScoreChanged(ATacticaPlayerState* PlayerState, int32 Score) const
{
	if (!ensure(TacticaGameState))
	{
		return;
	}

	if (Score >= TacticaGameState->GetWinningScore())
	{
		TacticaGameState->Multicast_BroadcastWinner(PlayerState);
	}
}

void ATacticaGameMode::FindGameState()
{
	if (ATacticaGameState* AsGameState = GetGameState<ATacticaGameState>(); ensure(AsGameState))
	{
		TacticaGameState = AsGameState;
	}
}
