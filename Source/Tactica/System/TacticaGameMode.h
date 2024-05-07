#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TacticaGameMode.generated.h"

class ATacticaGameState;
class ATacticaPlayerState;

UCLASS(Abstract, minimalapi)
class ATacticaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 WinningScore = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MinNumberOfPlayers = 2;

	UPROPERTY(Transient)
	ATacticaGameState* TacticaGameState = nullptr;

private:
	void OnPlayerStateSpawned(ATacticaPlayerState* PlayerState) const;
	void OnPlayerStateScoreChanged(ATacticaPlayerState* PlayerState, int32 Score) const;

	void FindGameState();

	int32 NumberOfPlayers = 0;
	
	FDelegateHandle PlayerStateSpawnedHandle;
	
};



