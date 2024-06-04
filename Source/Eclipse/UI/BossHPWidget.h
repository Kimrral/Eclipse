// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "BossHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBossHPWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdateHPWidget(float NewCurrentHP, float MaxHP);
	UFUNCTION()
	void UpdateShieldWidget(float NewCurrentShield, float MaxShield) const;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TObjectPtr<class UProgressBar> ShieldProgressBar;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TObjectPtr<class UTextBlock> HpStat;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TObjectPtr<class UTextBlock> MaxHpStat;
	
	FString GetHpStatText() const;

	FString GetMaxHpStatText() const;

	UPROPERTY()
	bool MaxHpAlreadySet;

	UPROPERTY(Transient)
	float WidgetCurrentHP;

	UPROPERTY(Transient)
	float WidgetMaxHP;
	
	
};
