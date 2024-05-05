#include "TacticaHUD.h"

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
			ScoreBoardWidget->AddToViewport(1);
		}
	}
}
