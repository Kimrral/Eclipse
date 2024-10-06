// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Manager/InventoryController.h"
#include "InventoryOverlayController.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UInventoryOverlayController : public UInventoryControllerComponent
{
	GENERATED_BODY()
	
public:
	UInventoryOverlayController();
    
	// Functions for handling non-interactive overlay elements
	void DisplayOverlayInfo() const;
	
protected:
	virtual void BeginPlay() override;

private:
	// Reference to the Player's Stat Component
	UPROPERTY()
	class UPlayerCharacterStatComponent* PlayerStatComponent;

	// Reference to the Inventory Widget
	UPROPERTY()
	class UInventoryWidget* InventoryWidget;

	void UpdateHpInfo() const;
};
