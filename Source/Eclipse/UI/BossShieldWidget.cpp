// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/UI/BossShieldWidget.h"

#include "Components/ProgressBar.h"

void UBossShieldWidget::UpdateShieldWidget(const float NewCurrentShield, const float MaxShield) const
{
	// 함수의 인자로 전달된 새로운 현재 실드 값을 WidgetCurrentShield 변수에 저장합니다.
	const float WidgetCurrentShield = NewCurrentShield;

	// 함수의 인자로 전달된 새로운 최대 실드 값을 WidgetMaxShield 변수에 저장합니다.
	const float WidgetMaxShield = MaxShield;

	// ShieldProgressBar의 퍼센트를 현재 실드와 최대 실드의 비율로 설정합니다.
	// WidgetCurrentShield를 WidgetMaxShield로 나누어 현재 실드 비율을 계산한 후,
	// SetPercent 함수로 ShieldProgressBar에 적용합니다.
	ShieldProgressBar->SetPercent(WidgetCurrentShield / WidgetMaxShield);
}
