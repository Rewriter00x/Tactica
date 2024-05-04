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
		FTimerDelegate Delegate;
		Delegate.BindWeakLambda(this, [this]
		{
			GetWorld()->GetAuthGameMode()->RestartPlayer(this);
		});
		
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, Delegate, 2.f, false);
	}
}
