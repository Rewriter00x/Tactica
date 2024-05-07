#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TacticaPlayerState.generated.h"

class ATacticaPlayerState;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPlayerScoreChanged, ATacticaPlayerState*, int32)

UCLASS()
class TACTICA_API ATacticaPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	FORCEINLINE int32 GetPlayerScore() const { return PlayerScore; }
	
	void AddPlayerScore();
	void ClearPlayerScore();

	FOnPlayerScoreChanged OnPlayerScoreChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerScore)
	int32 PlayerScore = 0;

private:
	UFUNCTION()
	void OnRep_PlayerScore();
	
};
