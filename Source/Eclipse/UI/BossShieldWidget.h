// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossShieldWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBossShieldWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TObjectPtr<class UProgressBar> ShieldProgressBar;
	
	UFUNCTION()
	void UpdateShieldWidget(float NewCurrentShield, float MaxShield) const;
	
};
