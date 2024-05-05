#include "TacticaHUD.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ScoreBoardWidget.h"
#include "Blueprint/UserWidget.h"

void ATacticaHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidgetClass)
	{
		MainWidget = CreateWidget(GetOwningPlayerController(), MainWidgetClass);
		if (MainWidget)
		{
			MainWidget->AddToViewport(0);
		}
	}
	
	if (ScoreBoardWidgetClass)
	{
		ScoreBoardWidget = CreateWidget<UScoreBoardWidget>(GetOwningPlayerController(), ScoreBoardWidgetClass);
		if (ScoreBoardWidget)
		{
			ScoreBoardWidget->SetVisibility(ESlateVisibility::Collapsed);
			ScoreBoardWidget->AddToViewport(1);
		}
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetOwningPlayerController()->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(ShowMappingContext, 2);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(GetOwningPlayerController()->InputComponent))
	{
		EnhancedInputComponent->BindAction(ShowAction, ETriggerEvent::Triggered, this, &ThisClass::ChangeScoreBoardVisibility);
	}
}

void ATacticaHUD::ChangeScoreBoardVisibility(const FInputActionValue& Value)
{
	const bool Visible = Value.Get<bool>();

	if (ScoreBoardWidget)
	{
		ScoreBoardWidget->SetVisibility(Visible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
