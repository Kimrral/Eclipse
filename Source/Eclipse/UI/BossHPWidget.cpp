// Fill out your copyright notice in the Description page of Project Settings.


#include "BossHPWidget.h"

#include "Components/ProgressBar.h"

void UBossHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MaxHpAlreadySet = false;
}

void UBossHPWidget::UpdateHPWidget(const float NewCurrentHP, const float MaxHP)
{
	if (GetOwningPlayerPawn()->IsLocallyControlled())
	{
		WidgetCurrentHP = NewCurrentHP;
		WidgetMaxHP = MaxHP;


		ProgressBar->SetPercent(WidgetCurrentHP / WidgetMaxHP);


		if (!MaxHpAlreadySet)
		{
			MaxHpStat->SetText(FText::FromString(GetMaxHpStatText()));
			HpStat->SetText(FText::FromString(GetHpStatText()));
			MaxHpAlreadySet = true;
		}
		else
		{
			HpStat->SetText(FText::FromString(GetHpStatText()));
		}
	}
}

void UBossHPWidget::UpdateShieldWidget(const float NewCurrentShield, const float MaxShield) const
{
	if (GetOwningPlayerPawn()->IsLocallyControlled())
	{
		const float WidgetCurrentShield = NewCurrentShield;
		const float WidgetMaxShield = MaxShield;

		ShieldProgressBar->SetPercent(WidgetCurrentShield / WidgetMaxShield);
	}
}

FString UBossHPWidget::GetMaxHpStatText() const
{
	return FString::Printf(TEXT("%.0f"), WidgetMaxHP);
}


FString UBossHPWidget::GetHpStatText() const
{
	return FString::Printf(TEXT("%.0f"), WidgetCurrentHP);
}
