// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/RadialSlider.h"
#include "Components/WidgetSwitcher.h"
#include "WeaponInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UWeaponInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_3;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_4;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_5;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_6;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_7;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_8;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_9;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_10;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_11;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_12;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_13;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_14;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_15;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_16;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_17;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_18;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider_19;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_Weapon;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* MissionCheck1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* MissionCheck2;
	
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* LackMission;
	
	
	UPROPERTY()
	double weaponHoldPercent = 0.f;

	
};
