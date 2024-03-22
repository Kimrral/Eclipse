// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UDamageWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* damageText;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* DamageFloat;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* rifleBulletImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* sniperBulletImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* pistolBulletImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* M249BulletImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* ShieldImage;
	
	UPROPERTY()
	int damage;
	
};
