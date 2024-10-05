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

	UPROPERTY(BlueprintReadWrite)
	int32 DraggedIndex;
	UPROPERTY(BlueprintReadWrite)
	int32 DroppedIndex;

	UFUNCTION(BlueprintImplementableEvent)
	void AddToInventoryLocal(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct);

	UFUNCTION(BlueprintImplementableEvent)
	void SyncInventoryDataFromServer(const TArray<FPlayerInventoryStruct>& ServerInventoryData);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
