#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TacticaGameState.generated.h"

class ATacticaPlayerState;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateSpawn, ATacticaPlayerState*);

UCLASS()
class TACTICA_API ATacticaGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	FOnPlayerStateSpawn OnPlayerStateSpawned;
	FOnPlayerStateSpawn OnPlayerStateDeSpawned;
	
};
