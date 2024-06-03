// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/UI/BossShieldWidget.h"

#include "Components/ProgressBar.h"

void UBossShieldWidget::UpdateShieldWidget(const float NewCurrentShield, const float MaxShield) const
{
	if(GetOwningPlayerPawn()->IsLocallyControlled())
	{
		const float WidgetCurrentShield = NewCurrentShield;
		const float WidgetMaxShield = MaxShield;

		ShieldProgressBar->SetPercent(WidgetCurrentShield / WidgetMaxShield);
	}
}
