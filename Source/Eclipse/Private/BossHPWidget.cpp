// Fill out your copyright notice in the Description page of Project Settings.


#include "BossHPWidget.h"

#include "Components/ProgressBar.h"

void UBossHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	progressBar->SetPercent(1);
}

void UBossHPWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//progressBar->SetPercent()
}
