#pragma once

#include "CoreMinimal.h"
#include "GameStatus.h"
#include "GameFramework/GameStateBase.h"
#include "TacticaGameState.generated.h"

class ATacticaPlayerState;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateSpawn, ATacticaPlayerState*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameStatusChanged, EGameStatus);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWinningScoreChanged, int32);

UCLASS()
class TACTICA_API ATacticaGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE int32 GetWinningScore() const { return WinningScore; }
	FORCEINLINE int32 GetMinNumberOfPlayers() const { return MinNumberOfPlayers; }
	FORCEINLINE int32 GetNumberOfPlayers() const { return NumberOfPlayers; }
	FORCEINLINE EGameStatus GetGameStatus() const { return GameStatus; }
	FORCEINLINE ATacticaPlayerState* GetWinnerPlayerState() const { return WinnerPlayerState; }

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BroadcastWinner(ATacticaPlayerState* PlayerState);

	void SetWinningScore(const int32 Score);
	void SetGameStatus(const EGameStatus Status);
	void SetMinNumberOfPlayers(const int32 Number);
	void SetNumberOfPlayers(const int32 Number);
	
	FOnPlayerStateSpawn OnPlayerStateSpawned;
	FOnPlayerStateSpawn OnPlayerStateDeSpawned;
	
	FOnGameStatusChanged OnGameStatusChanged;
	FOnWinningScoreChanged OnWinningScoreChanged;
	FOnWinningScoreChanged OnMinNumberOfPlayersChanged;
	FOnWinningScoreChanged OnNumberOfPlayersChanged;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_WinningScore)
	int32 WinningScore = 0;
	
	UPROPERTY(ReplicatedUsing=OnRep_MinNumberOfPlayers)
	int32 MinNumberOfPlayers = 0;
	
	UPROPERTY(ReplicatedUsing=OnRep_NumberOfPlayers)
	int32 NumberOfPlayers = 0;

	UPROPERTY(ReplicatedUsing=OnRep_GameStatus)
	EGameStatus GameStatus = EGameStatus::None;

	UPROPERTY(Transient)
	ATacticaPlayerState* WinnerPlayerState = nullptr;

private:
	UFUNCTION()
	void OnRep_WinningScore();
	
	UFUNCTION()
	void OnRep_MinNumberOfPlayers();

	UFUNCTION()
	void OnRep_NumberOfPlayers();

	UFUNCTION()
	void OnRep_GameStatus();
	
};
