// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "TradeWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UTradeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Construction(APlayerCharacter* PlayerCharacter);

	UFUNCTION()
	void UpdateRoubleText(const int32 CurrentRoubles) const;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* SellButton;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* CurrentRouble;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* IncomeRouble;
	
};
