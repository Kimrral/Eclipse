// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TabWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UTabWidget : public UUserWidget
{
	GENERATED_BODY()

public:		
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* WeaponText1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* WeaponText2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* CurBulletText1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* CurBulletText2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* BulletText1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* BulletText2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* MaxBulletText1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* MaxBulletText2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* MaxBulletText3;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* MaxBulletText4;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage1_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage1_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage1_3;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage2_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage2_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage2_3;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage1_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage1_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage1_3;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage2_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage2_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage2_3;
	
	
};
