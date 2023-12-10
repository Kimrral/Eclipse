// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInfoWidget.h"

void UWeaponInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	weaponHoldPercent=FMath::Clamp(weaponHoldPercent-0.003, 0, 1);
	
	progressSlider->SetValue(weaponHoldPercent);
	progressSlider_1->SetValue(weaponHoldPercent);
	progressSlider_2->SetValue(weaponHoldPercent);
	progressSlider_3->SetValue(weaponHoldPercent);
	progressSlider_4->SetValue(weaponHoldPercent);
	progressSlider_5->SetValue(weaponHoldPercent);

}
