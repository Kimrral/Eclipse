// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInfoWidget.h"

void UWeaponInfoWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	progressSlider->SetValue();
}
