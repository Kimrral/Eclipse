// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UInventory : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* UseButton;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* CloseButton;

	UFUNCTION(BlueprintCallable)
	void ButtonConstruct();

	UFUNCTION(BlueprintImplementableEvent)
	void UseButtonClicked();

	UFUNCTION(BlueprintImplementableEvent)
	void CloseButtonClicked();
	
};
