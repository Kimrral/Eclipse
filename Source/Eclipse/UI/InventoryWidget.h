// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	class AEclipsePlayerController* PlayerController;

	UPROPERTY()
	class APlayerCharacter* OwningPlayer;

	UFUNCTION(BlueprintImplementableEvent)
	void AddToInventoryLocal(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct);

	UFUNCTION(BlueprintImplementableEvent)
	void SyncInventoryDataFromServer(const TArray<FPlayerInventoryStruct>& ServerInventoryData);

	// Bind Controller to update the widget
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void BindStatController(UInventoryStatController* StatController);

	// Update widget from controller
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UpdateStatDisplay();

	// Updates the HP display on the widget
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateHpDisplay(float CurrentHp, float MaxHp);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory", meta = (BindWidget))
	class UTextBlock* CurrentHpText;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory", meta = (BindWidget))
	class UTextBlock* MaxHpText;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory", meta = (BindWidget))
	class UTextBlock* CurrentRoubleText;
	
	UPROPERTY(BlueprintReadWrite)
	int32 DraggedIndex;
	UPROPERTY(BlueprintReadWrite)
	int32 DroppedIndex;


private:
	// Reference to the stat controller
	UPROPERTY()
	UInventoryStatController* BoundStatController;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
