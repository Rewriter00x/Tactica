#include "AmmoCounterWidget.h"

#include "Components/TextBlock.h"
#include "Tactica/Character/TacticaCharacter.h"
#include "Tactica/Weapon/Weapon.h"

void UAmmoCounterWidget::BindPawn(APawn* OldPawn, APawn* NewPawn)
{
	Super::BindPawn(OldPawn, NewPawn);

	OnSelectedWeaponAmmoChanged(0, 0);

	if (ATacticaCharacter* Character = Cast<ATacticaCharacter>(OldPawn))
	{
		Character->OnSelectedWeaponChanged.Remove(WeaponDelegateHandle);
	}

	if (ATacticaCharacter* Character = Cast<ATacticaCharacter>(NewPawn))
	{
		WeaponDelegateHandle = Character->OnSelectedWeaponChanged.AddUObject(this, &UAmmoCounterWidget::OnSelectedWeaponChanged);
		OnSelectedWeaponChanged(Character->GetSelectedWeapon());
	}
}

void UAmmoCounterWidget::OnSelectedWeaponChanged(AWeapon* Weapon)
{
	if (SelectedWeapon)
	{
		SelectedWeapon->OnWeaponAmmoChanged.Remove(AmmoDelegateHandle);
	}

	if (Weapon)
	{
		SelectedWeapon = Weapon;
		AmmoDelegateHandle = SelectedWeapon->OnWeaponAmmoChanged.AddUObject(this, &UAmmoCounterWidget::OnSelectedWeaponAmmoChanged);
		OnSelectedWeaponAmmoChanged(SelectedWeapon->GetLoadedAmmo(), SelectedWeapon->GetSpareAmmo());
	}
	
	SetVisibility(IsValid(Weapon) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UAmmoCounterWidget::OnSelectedWeaponAmmoChanged(int32 LoadedAmmo, int32 SpareAmmo) const
{
	AmmoTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), LoadedAmmo, SpareAmmo)));
}
