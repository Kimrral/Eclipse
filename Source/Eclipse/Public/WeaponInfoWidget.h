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
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URadialSlider* progressSlider;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_Weapon;

	
};
