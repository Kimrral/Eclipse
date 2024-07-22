// Fill out your copyright notice in the Description page of Project Settings.


#include "BossHPWidget.h"

#include "Components/ProgressBar.h"

void UBossHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// MaxHpAlreadySet 변수를 false로 초기화합니다.
	MaxHpAlreadySet = false;
}

void UBossHPWidget::UpdateHPWidget(const float NewCurrentHP, const float MaxHP)
{
	// 현재 플레이어의 폰이 로컬에서 제어되고 있는지 확인합니다.
	if (GetOwningPlayerPawn()->IsLocallyControlled())
	{
		// 위젯의 현재 HP와 최대 HP를 업데이트합니다.
		WidgetCurrentHP = NewCurrentHP;
		WidgetMaxHP = MaxHP;

		// ProgressBar의 퍼센트를 현재 HP와 최대 HP의 비율로 설정합니다.
		ProgressBar->SetPercent(WidgetCurrentHP / WidgetMaxHP);

		// MaxHpAlreadySet 변수가 false일 경우
		if (!MaxHpAlreadySet)
		{
			// 최대 HP와 현재 HP를 표시하는 텍스트를 업데이트하고 MaxHpAlreadySet 변수를 true로 설정합니다.
			MaxHpStat->SetText(FText::FromString(GetMaxHpStatText()));
			HpStat->SetText(FText::FromString(GetHpStatText()));
			MaxHpAlreadySet = true;
		}
		else
		{
			// MaxHpAlreadySet 변수가 true일 경우 현재 HP 텍스트만 업데이트합니다.
			HpStat->SetText(FText::FromString(GetHpStatText()));
		}
	}
}

void UBossHPWidget::UpdateShieldWidget(const float NewCurrentShield, const float MaxShield) const
{
	// 현재 플레이어의 폰이 로컬에서 제어되고 있는지 확인합니다.
	if (GetOwningPlayerPawn()->IsLocallyControlled())
	{
		// 위젯의 현재 실드와 최대 실드를 업데이트합니다.
		const float WidgetCurrentShield = NewCurrentShield;
		const float WidgetMaxShield = MaxShield;

		// ShieldProgressBar의 퍼센트를 현재 실드와 최대 실드의 비율로 설정합니다.
		ShieldProgressBar->SetPercent(WidgetCurrentShield / WidgetMaxShield);
	}
}

FString UBossHPWidget::GetMaxHpStatText() const
{
	// 최대 HP를 텍스트 형식으로 반환합니다.
	return FString::Printf(TEXT("%.0f"), WidgetMaxHP);
}

FString UBossHPWidget::GetHpStatText() const
{
	// 현재 HP를 텍스트 형식으로 반환합니다.
	return FString::Printf(TEXT("%.0f"), WidgetCurrentHP);
}
