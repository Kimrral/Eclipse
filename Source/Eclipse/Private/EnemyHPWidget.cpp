// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHPWidget.h"

#include "Components/Image.h"

void UEnemyHPWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	HPBar->SetBrushFromMaterial(HPBarMatInt);
}

void UEnemyHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HPdynamicMat = HPBar->GetDynamicMaterial();

	
}
