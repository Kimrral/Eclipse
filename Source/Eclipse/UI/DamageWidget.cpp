// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageWidget.h"

#include "Components/TextBlock.h"

void UDamageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	damageText->SetText(FText::AsNumber(damage));
}
