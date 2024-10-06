// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Eclipse/Manager/InventoryController.h"
#include "InventoryStatController.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UInventoryStatController : public UInventoryControllerComponent
{
	GENERATED_BODY()
	
public:
	UInventoryStatController();

protected:
	virtual void BeginPlay() override;

private:
	// Player's Stat Component reference
	UPROPERTY()
	class UPlayerCharacterStatComponent* PlayerStatComponent;

public:
	// Function to get and display player's stats in the inventory widget
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetCurrentHp() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetMaxHp() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	float GetCurrentRouble() const;
	
};
