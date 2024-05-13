#include "TacticaPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Tactica/System/TacticaGameState.h"

void ATacticaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerScore);
}

void ATacticaPlayerState::AddPlayerScore()
{
	++PlayerScore;
	OnPlayerScoreChanged.Broadcast(this, PlayerScore);
}

void ATacticaPlayerState::ClearPlayerScore()
{
	PlayerScore = 0;
	OnPlayerScoreChanged.Broadcast(this, PlayerScore);
}

void ATacticaPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (const ATacticaGameState* GameState = Cast<ATacticaGameState>(GetWorld()->GetGameState()))
	{
		GameState->OnPlayerStateSpawned.Broadcast(this);
	}
}

void ATacticaPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const ATacticaGameState* GameState = Cast<ATacticaGameState>(GetWorld()->GetGameState()))
	{
		GameState->OnPlayerStateDeSpawned.Broadcast(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ATacticaPlayerState::OnRep_PlayerScore()
{
	OnPlayerScoreChanged.Broadcast(this, PlayerScore);
}
