#include "TacticaPlayerController.h"

#include "GameFramework/GameModeBase.h"

void ATacticaPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	NewPawn->OnDestroyed.AddDynamic(this, &ATacticaPlayerController::OnPossessedPawnDestroyed);
}

void ATacticaPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ATacticaPlayerController::OnPossessedPawnDestroyed(AActor* DestroyedPawn)
{
	if (HasAuthority())
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, [this]
		{
			GetWorld()->GetAuthGameMode()->RestartPlayer(this);
		}, 2.f, false);
	}
}
