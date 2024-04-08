// Fill out your copyright notice in the Description page of Project Settings.


#include "BossHPWidget.h"

#include "Components/ProgressBar.h"

void UBossHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	progressBar->SetPercent(1);
	shieldProgressBar->SetPercent(1);
}

void UBossHPWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
