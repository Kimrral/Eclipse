// Fill out your copyright notice in the Description page of Project Settings.


#include "InformationWidget.h"

#include "PlayerCharacter.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UInformationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	owner =  Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

}

void UInformationWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateAmmo();

}

void UInformationWidget::UpdateAmmo()
{
	if(owner->weaponArray[0]==true)
	{
		currentAmmo->SetText(FText::AsNumber(owner->curRifleAmmo));
		maxAmmo->SetText(FText::AsNumber(owner->maxRifleAmmo));
		AmmoType->SetText(FText::FromString("5.56mm"));
	}
	else if(owner->weaponArray[1]==true)
	{
		currentAmmo->SetText(FText::AsNumber(owner->curSniperAmmo));
		maxAmmo->SetText(FText::AsNumber(owner->maxSniperAmmo));
		AmmoType->SetText(FText::FromString("7.62mm"));
	}
	else if(owner->weaponArray[2]==true)
	{
		currentAmmo->SetText(FText::AsNumber(owner->curPistolAmmo));
		maxAmmo->SetText(FText::AsNumber(owner->maxPistolAmmo));
		AmmoType->SetText(FText::FromString("9mm"));
	}
	else if(owner->weaponArray[3]==true)
	{
		currentAmmo->SetText(FText::AsNumber(owner->curM249Ammo));
		maxAmmo->SetText(FText::AsNumber(owner->maxM249Ammo));
		AmmoType->SetText(FText::FromString("7.62mm"));
	}
}
