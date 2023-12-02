// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UEnemyHPWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UImage* HPBar;
	
	UPROPERTY(EditAnywhere)
	class UMaterialInterface* HPBarMatInt;

	UPROPERTY()	
	class UMaterialInstanceDynamic* HPdynamicMat;

	UPROPERTY()
	float coolDownTimeFloat;
	
};
