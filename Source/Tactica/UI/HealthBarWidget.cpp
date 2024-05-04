#include "HealthBarWidget.h"

#include "Components/ProgressBar.h"
#include "Tactica/Character/TacticaCharacter.h"

void UHealthBarWidget::BindPawn(APawn* OldPawn, APawn* NewPawn)
{
	Super::BindPawn(OldPawn, NewPawn);
	
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
