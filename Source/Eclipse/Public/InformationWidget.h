// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InformationWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UInformationWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void UpdateAmmo();

	UFUNCTION()
	void UpdateAmmo_Secondary();

	UPROPERTY()
	class APlayerCharacter* owner;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* currentAmmo;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* maxAmmo;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* AmmoType;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* rifleBulletImage;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* sniperBulletImage;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* pistolBulletImage;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* M249BulletImage;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class URetainerBox* MaterialRetainerBox;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* RifleIcon;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* SniperIcon;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* PistolIcon;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* M249Icon;

	

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* currentAmmo_1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* maxAmmo_1;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* AmmoType_1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* rifleBulletImage_1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* sniperBulletImage_1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* pistolBulletImage_1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* M249BulletImage_1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* RifleIcon_1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* SniperIcon_1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* PistolIcon_1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* M249Icon_1;
};
