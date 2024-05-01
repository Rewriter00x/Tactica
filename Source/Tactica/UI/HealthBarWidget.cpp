#include "HealthBarWidget.h"

#include "Components/ProgressBar.h"
#include "Tactica/Character/TacticaCharacter.h"

void UHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		BindPawn(nullptr, Pawn);
	}
	
	if (APlayerController* Controller = GetOwningPlayer())
	{
		Controller->OnPossessedPawnChanged.AddDynamic(this, &UHealthBarWidget::BindPawn);
	}
}

void UHealthBarWidget::NativeDestruct()
{
	if (APlayerController* Controller = GetOwningPlayer())
	{
		Controller->OnPossessedPawnChanged.RemoveDynamic(this, &UHealthBarWidget::BindPawn);
	}
	
	Super::NativeDestruct();
}

void UHealthBarWidget::BindPawn(APawn* OldPawn, APawn* NewPawn)
{
	if (ATacticaCharacter* Character = Cast<ATacticaCharacter>(OldPawn))
	{
		Character->OnHealthChanged.Remove(DelegateHandle);
	}

	if (ATacticaCharacter* Character = Cast<ATacticaCharacter>(NewPawn))
	{
		DelegateHandle = Character->OnHealthChanged.AddUObject(this, &UHealthBarWidget::OnPossessedPawnHealthChanged);
		OnPossessedPawnHealthChanged(Character->GetHealth(), 0.f);
	}
}

void UHealthBarWidget::OnPossessedPawnHealthChanged(float NewValue, float OldValue) const
{
	HealthProgressBar->SetPercent(NewValue / 100.f);
}
