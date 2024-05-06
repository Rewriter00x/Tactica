#include "CrosshairWidget.h"

#include "Components/Image.h"
#include "Tactica/Character/TacticaCharacter.h"
#include "Tactica/Weapon/Weapon.h"

void UCrosshairWidget::BindPawn(APawn* OldPawn, APawn* NewPawn)
{
	Super::BindPawn(OldPawn, NewPawn);

	if (ATacticaCharacter* Character = Cast<ATacticaCharacter>(OldPawn))
	{
		Character->OnSelectedWeaponChanged.Remove(DelegateHandle);
	}

	if (ATacticaCharacter* Character = Cast<ATacticaCharacter>(NewPawn))
	{
		DelegateHandle = Character->OnSelectedWeaponChanged.AddUObject(this, &UCrosshairWidget::OnPossessedPawnSelectedWeaponChanged);
		OnPossessedPawnSelectedWeaponChanged(Character->GetSelectedWeapon());
	}
}

void UCrosshairWidget::OnPossessedPawnSelectedWeaponChanged(AWeapon* NewWeapon) const
{
	CrosshairImage->SetVisibility(IsValid(NewWeapon) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
