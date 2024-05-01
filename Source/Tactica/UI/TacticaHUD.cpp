#include "TacticaHUD.h"

#include "Blueprint/UserWidget.h"

void ATacticaHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!MainWidgetClass)
	{
		return;
	}
	
	MainWidget = CreateWidget(GetOwningPlayerController(), MainWidgetClass);
	if (MainWidget)
	{
		MainWidget->AddToViewport();
	}
}
