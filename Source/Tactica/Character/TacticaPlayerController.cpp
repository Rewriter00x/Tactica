#include "TacticaPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

void ATacticaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(EscapeMappingContext, 3);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(EscapeAction, ETriggerEvent::Triggered, this, &ThisClass::ExitToMenu);
	}
}

void ATacticaPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(EscapeMappingContext);
	}
	
	Super::EndPlay(EndPlayReason);
}

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

void ATacticaPlayerController::ExitToMenu()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, MenuLevel, true);
}
