// Fill out your copyright notice in the Description page of Project Settings.


#include "InformationWidget.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Components/Image.h"
#include "Components/RetainerBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UInformationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	owner = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	UpdateAmmo();
	UpdateAmmo_Secondary();

	FTimerHandle ammoUpdateHandle;
	GetWorld()->GetTimerManager().SetTimer(ammoUpdateHandle, this, &UInformationWidget::UpdateAmmo, 0.01f, true);
}

void UInformationWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UInformationWidget::UpdateAmmo()
{
	if (owner)
	{
		if (owner->weaponArray[0] == true)
		{
			currentAmmo->SetText(FText::AsNumber(owner->curRifleAmmo));
			maxAmmo->SetText(FText::AsNumber(owner->maxRifleAmmo));
			AmmoType->SetText(FText::FromString("5.56mm"));

			rifleBulletImage->SetVisibility(ESlateVisibility::Visible);
			sniperBulletImage->SetVisibility(ESlateVisibility::Hidden);
			pistolBulletImage->SetVisibility(ESlateVisibility::Hidden);
			M249BulletImage->SetVisibility(ESlateVisibility::Hidden);

			RifleIcon->SetVisibility(ESlateVisibility::Visible);
			SniperIcon->SetVisibility(ESlateVisibility::Hidden);
			PistolIcon->SetVisibility(ESlateVisibility::Hidden);
			M249Icon->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (owner->weaponArray[1] == true)
		{
			currentAmmo->SetText(FText::AsNumber(owner->curSniperAmmo));
			maxAmmo->SetText(FText::AsNumber(owner->maxSniperAmmo));
			AmmoType->SetText(FText::FromString("7.92mm"));

			rifleBulletImage->SetVisibility(ESlateVisibility::Hidden);
			sniperBulletImage->SetVisibility(ESlateVisibility::Visible);
			pistolBulletImage->SetVisibility(ESlateVisibility::Hidden);
			M249BulletImage->SetVisibility(ESlateVisibility::Hidden);

			RifleIcon->SetVisibility(ESlateVisibility::Hidden);
			SniperIcon->SetVisibility(ESlateVisibility::Visible);
			PistolIcon->SetVisibility(ESlateVisibility::Hidden);
			M249Icon->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (owner->weaponArray[2] == true)
		{
			currentAmmo->SetText(FText::AsNumber(owner->curPistolAmmo));
			maxAmmo->SetText(FText::AsNumber(owner->maxPistolAmmo));
			AmmoType->SetText(FText::FromString("9mm"));

			rifleBulletImage->SetVisibility(ESlateVisibility::Hidden);
			sniperBulletImage->SetVisibility(ESlateVisibility::Hidden);
			pistolBulletImage->SetVisibility(ESlateVisibility::Visible);
			M249BulletImage->SetVisibility(ESlateVisibility::Hidden);

			RifleIcon->SetVisibility(ESlateVisibility::Hidden);
			SniperIcon->SetVisibility(ESlateVisibility::Hidden);
			PistolIcon->SetVisibility(ESlateVisibility::Visible);
			M249Icon->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (owner->weaponArray[3] == true)
		{
			currentAmmo->SetText(FText::AsNumber(owner->curM249Ammo));
			maxAmmo->SetText(FText::AsNumber(owner->maxM249Ammo));
			AmmoType->SetText(FText::FromString("7.62mm"));

			rifleBulletImage->SetVisibility(ESlateVisibility::Hidden);
			sniperBulletImage->SetVisibility(ESlateVisibility::Hidden);
			pistolBulletImage->SetVisibility(ESlateVisibility::Hidden);
			M249BulletImage->SetVisibility(ESlateVisibility::Visible);

			RifleIcon->SetVisibility(ESlateVisibility::Hidden);
			SniperIcon->SetVisibility(ESlateVisibility::Hidden);
			PistolIcon->SetVisibility(ESlateVisibility::Hidden);
			M249Icon->SetVisibility(ESlateVisibility::Visible);
		}

		MaterialRetainerBox->RequestRender();
	}
}

void UInformationWidget::UpdateAmmo_Secondary()
{
	if (owner)
	{
		if (owner->curWeaponSlotNumber == 1)
		{
			if (owner->equippedWeaponStringArray[1] == FString("Rifle"))
			{
				currentAmmo_1->SetText(FText::AsNumber(owner->curRifleAmmo));
				maxAmmo_1->SetText(FText::AsNumber(owner->maxRifleAmmo));
				AmmoType_1->SetText(FText::FromString("5.56mm"));

				rifleBulletImage_1->SetVisibility(ESlateVisibility::Visible);
				sniperBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				pistolBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				M249BulletImage_1->SetVisibility(ESlateVisibility::Hidden);

				RifleIcon_1->SetVisibility(ESlateVisibility::Visible);
				SniperIcon_1->SetVisibility(ESlateVisibility::Hidden);
				PistolIcon_1->SetVisibility(ESlateVisibility::Hidden);
				M249Icon_1->SetVisibility(ESlateVisibility::Hidden);
			}
			else if (owner->equippedWeaponStringArray[1] == FString("Sniper"))
			{
				currentAmmo_1->SetText(FText::AsNumber(owner->curSniperAmmo));
				maxAmmo_1->SetText(FText::AsNumber(owner->maxSniperAmmo));
				AmmoType_1->SetText(FText::FromString("7.92mm"));

				rifleBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				sniperBulletImage_1->SetVisibility(ESlateVisibility::Visible);
				pistolBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				M249BulletImage_1->SetVisibility(ESlateVisibility::Hidden);

				RifleIcon_1->SetVisibility(ESlateVisibility::Hidden);
				SniperIcon_1->SetVisibility(ESlateVisibility::Visible);
				PistolIcon_1->SetVisibility(ESlateVisibility::Hidden);
				M249Icon_1->SetVisibility(ESlateVisibility::Hidden);
			}
			else if (owner->equippedWeaponStringArray[1] == FString("Pistol"))
			{
				currentAmmo_1->SetText(FText::AsNumber(owner->curPistolAmmo));
				maxAmmo_1->SetText(FText::AsNumber(owner->maxPistolAmmo));
				AmmoType_1->SetText(FText::FromString("9mm"));

				rifleBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				sniperBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				pistolBulletImage_1->SetVisibility(ESlateVisibility::Visible);
				M249BulletImage_1->SetVisibility(ESlateVisibility::Hidden);

				RifleIcon_1->SetVisibility(ESlateVisibility::Hidden);
				SniperIcon_1->SetVisibility(ESlateVisibility::Hidden);
				PistolIcon_1->SetVisibility(ESlateVisibility::Visible);
				M249Icon_1->SetVisibility(ESlateVisibility::Hidden);
			}
			else if (owner->equippedWeaponStringArray[1] == FString("M249"))
			{
				currentAmmo_1->SetText(FText::AsNumber(owner->curM249Ammo));
				maxAmmo_1->SetText(FText::AsNumber(owner->maxM249Ammo));
				AmmoType_1->SetText(FText::FromString("7.62mm"));

				rifleBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				sniperBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				pistolBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				M249BulletImage_1->SetVisibility(ESlateVisibility::Visible);

				RifleIcon_1->SetVisibility(ESlateVisibility::Hidden);
				SniperIcon_1->SetVisibility(ESlateVisibility::Hidden);
				PistolIcon_1->SetVisibility(ESlateVisibility::Hidden);
				M249Icon_1->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			if (owner->equippedWeaponStringArray[0] == FString("Rifle"))
			{
				currentAmmo_1->SetText(FText::AsNumber(owner->curRifleAmmo));
				maxAmmo_1->SetText(FText::AsNumber(owner->maxRifleAmmo));
				AmmoType_1->SetText(FText::FromString("5.56mm"));

				rifleBulletImage_1->SetVisibility(ESlateVisibility::Visible);
				sniperBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				pistolBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				M249BulletImage_1->SetVisibility(ESlateVisibility::Hidden);

				RifleIcon_1->SetVisibility(ESlateVisibility::Visible);
				SniperIcon_1->SetVisibility(ESlateVisibility::Hidden);
				PistolIcon_1->SetVisibility(ESlateVisibility::Hidden);
				M249Icon_1->SetVisibility(ESlateVisibility::Hidden);
			}
			else if (owner->equippedWeaponStringArray[0] == FString("Sniper"))
			{
				currentAmmo_1->SetText(FText::AsNumber(owner->curSniperAmmo));
				maxAmmo_1->SetText(FText::AsNumber(owner->maxSniperAmmo));
				AmmoType_1->SetText(FText::FromString("7.92mm"));

				rifleBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				sniperBulletImage_1->SetVisibility(ESlateVisibility::Visible);
				pistolBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				M249BulletImage_1->SetVisibility(ESlateVisibility::Hidden);

				RifleIcon_1->SetVisibility(ESlateVisibility::Hidden);
				SniperIcon_1->SetVisibility(ESlateVisibility::Visible);
				PistolIcon_1->SetVisibility(ESlateVisibility::Hidden);
				M249Icon_1->SetVisibility(ESlateVisibility::Hidden);
			}
			else if (owner->equippedWeaponStringArray[0] == FString("Pistol"))
			{
				currentAmmo_1->SetText(FText::AsNumber(owner->curPistolAmmo));
				maxAmmo_1->SetText(FText::AsNumber(owner->maxPistolAmmo));
				AmmoType_1->SetText(FText::FromString("9mm"));

				rifleBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				sniperBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				pistolBulletImage_1->SetVisibility(ESlateVisibility::Visible);
				M249BulletImage_1->SetVisibility(ESlateVisibility::Hidden);

				RifleIcon_1->SetVisibility(ESlateVisibility::Hidden);
				SniperIcon_1->SetVisibility(ESlateVisibility::Hidden);
				PistolIcon_1->SetVisibility(ESlateVisibility::Visible);
				M249Icon_1->SetVisibility(ESlateVisibility::Hidden);
			}
			else if (owner->equippedWeaponStringArray[0] == FString("M249"))
			{
				currentAmmo_1->SetText(FText::AsNumber(owner->curM249Ammo));
				maxAmmo_1->SetText(FText::AsNumber(owner->maxM249Ammo));
				AmmoType_1->SetText(FText::FromString("7.62mm"));

				rifleBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				sniperBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				pistolBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
				M249BulletImage_1->SetVisibility(ESlateVisibility::Visible);

				RifleIcon_1->SetVisibility(ESlateVisibility::Hidden);
				SniperIcon_1->SetVisibility(ESlateVisibility::Hidden);
				PistolIcon_1->SetVisibility(ESlateVisibility::Hidden);
				M249Icon_1->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}
