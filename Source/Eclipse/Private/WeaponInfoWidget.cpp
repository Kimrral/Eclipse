// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInfoWidget.h"

void UWeaponInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

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
	progressSlider_6->SetValue(weaponHoldPercent);
	progressSlider_7->SetValue(weaponHoldPercent);
	progressSlider_8->SetValue(weaponHoldPercent);
	progressSlider_9->SetValue(weaponHoldPercent);
	progressSlider_10->SetValue(weaponHoldPercent);
	progressSlider_11->SetValue(weaponHoldPercent);
	progressSlider_12->SetValue(weaponHoldPercent);
	progressSlider_13->SetValue(weaponHoldPercent);
	progressSlider_14->SetValue(weaponHoldPercent);
	progressSlider_15->SetValue(weaponHoldPercent);
	progressSlider_16->SetValue(weaponHoldPercent);
	progressSlider_17->SetValue(weaponHoldPercent);
	progressSlider_18->SetValue(weaponHoldPercent);

}
