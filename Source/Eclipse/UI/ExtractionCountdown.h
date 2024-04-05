// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExtractionCountdown.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnExtractionSuccess);

/**
 * 
 */
UCLASS()
class ECLIPSE_API UExtractionCountdown : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void SetCountdownTimer();

	FOnExtractionSuccess ExtractionSuccessDele;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* CountdownValue;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* CountdownConstruct;

	UPROPERTY()
	float Countdown = 10.f;

	UPROPERTY()
	bool bExtraction = false;
};
