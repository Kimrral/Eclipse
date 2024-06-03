// Fill out your copyright notice in the Description page of Project Settings.


#include "BossHPWidget.h"

#include "Components/ProgressBar.h"

void UBossHPWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FString UBossHPWidget::GetHPStatText() const
{
	return FString::Printf(TEXT("%.0f/%0.f"), WidgetCurrentHP, WidgetMaxHP);
}

void UBossHPWidget::UpdateHPWidget(const float NewCurrentHP, const float MaxHP)
{
	if(GetOwningPlayerPawn()->IsLocallyControlled())
	{
		WidgetCurrentHP = NewCurrentHP;
		WidgetMaxHP = MaxHP;

		ProgressBar->SetPercent(WidgetCurrentHP / WidgetMaxHP);
		HpStat->SetText(FText::FromString(GetHPStatText()));
	}
}

void UBossHPWidget::UpdateShieldWidget(const float NewCurrentShield, const float MaxShield) const
{
	if(GetOwningPlayerPawn()->IsLocallyControlled())
	{
		const float WidgetCurrentShield = NewCurrentShield;
		const float WidgetMaxShield = MaxShield;

		ShieldProgressBar->SetPercent(WidgetCurrentShield / WidgetMaxShield);
	}
}
