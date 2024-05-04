#include "TacticaCharacterObserverWidget.h"

void UTacticaCharacterObserverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		BindPawn(nullptr, Pawn);
	}
	
	if (APlayerController* Controller = GetOwningPlayer())
	{
		Controller->OnPossessedPawnChanged.AddDynamic(this, &UTacticaCharacterObserverWidget::BindPawn);
	}
}

void UTacticaCharacterObserverWidget::NativeDestruct()
{
	if (APlayerController* Controller = GetOwningPlayer())
	{
		Controller->OnPossessedPawnChanged.RemoveDynamic(this, &UTacticaCharacterObserverWidget::BindPawn);
	}
	
	Super::NativeDestruct();
}

void UTacticaCharacterObserverWidget::BindPawn(APawn* OldPawn, APawn* NewPawn)
{
}
