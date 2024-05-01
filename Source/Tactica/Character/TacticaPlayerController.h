#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TacticaPlayerController.generated.h"

UCLASS()
class TACTICA_API ATacticaPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* NewPawn) override;
	virtual void OnUnPossess() override;

private:
	UFUNCTION()
	void OnPossessedPawnDestroyed(AActor* DestroyedPawn);
	
};
