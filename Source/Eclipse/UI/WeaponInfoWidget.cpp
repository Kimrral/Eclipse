// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInfoWidget.h"

void UWeaponInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWeaponInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	weaponHoldPercent = FMath::Clamp(weaponHoldPercent - 0.003, 0, 1);

	if (progressSlider->IsVisible()) progressSlider->SetValue(weaponHoldPercent);
	if (progressSlider_1->IsVisible()) progressSlider_1->SetValue(weaponHoldPercent);
	if (progressSlider_2->IsVisible()) progressSlider_2->SetValue(weaponHoldPercent);
	if (progressSlider_3->IsVisible()) progressSlider_3->SetValue(weaponHoldPercent);
	if (progressSlider_4->IsVisible()) progressSlider_4->SetValue(weaponHoldPercent);
	if (progressSlider_5->IsVisible()) progressSlider_5->SetValue(weaponHoldPercent);
	if (progressSlider_6->IsVisible()) progressSlider_6->SetValue(weaponHoldPercent);
	if (progressSlider_7->IsVisible()) progressSlider_7->SetValue(weaponHoldPercent);
	if (progressSlider_8->IsVisible()) progressSlider_8->SetValue(weaponHoldPercent);
	if (progressSlider_9->IsVisible()) progressSlider_9->SetValue(weaponHoldPercent);
	if (progressSlider_10->IsVisible()) progressSlider_10->SetValue(weaponHoldPercent);
	if (progressSlider_11->IsVisible()) progressSlider_11->SetValue(weaponHoldPercent);
	if (progressSlider_12->IsVisible()) progressSlider_12->SetValue(weaponHoldPercent);
	if (progressSlider_13->IsVisible()) progressSlider_13->SetValue(weaponHoldPercent);
	if (progressSlider_14->IsVisible()) progressSlider_14->SetValue(weaponHoldPercent);
	if (progressSlider_15->IsVisible()) progressSlider_15->SetValue(weaponHoldPercent);
	if (progressSlider_16->IsVisible()) progressSlider_16->SetValue(weaponHoldPercent);
	if (progressSlider_17->IsVisible()) progressSlider_17->SetValue(weaponHoldPercent);
	if (progressSlider_18->IsVisible()) progressSlider_18->SetValue(weaponHoldPercent);
	if (progressSlider_19->IsVisible()) progressSlider_19->SetValue(weaponHoldPercent);
	if (progressSlider_20->IsVisible()) progressSlider_20->SetValue(weaponHoldPercent);
	if (progressSlider_21->IsVisible()) progressSlider_21->SetValue(weaponHoldPercent);
	if (progressSlider_22->IsVisible()) progressSlider_22->SetValue(weaponHoldPercent);
	if (progressSlider_23->IsVisible()) progressSlider_23->SetValue(weaponHoldPercent);
	if (progressSlider_24->IsVisible()) progressSlider_24->SetValue(weaponHoldPercent);
	if (progressSlider_25->IsVisible()) progressSlider_25->SetValue(weaponHoldPercent);
	if (progressSlider_26->IsVisible()) progressSlider_26->SetValue(weaponHoldPercent);
	if (progressSlider_27->IsVisible()) progressSlider_27->SetValue(weaponHoldPercent);
	if (progressSlider_28->IsVisible()) progressSlider_28->SetValue(weaponHoldPercent);
	if (progressSlider_29->IsVisible()) progressSlider_29->SetValue(weaponHoldPercent);
}
