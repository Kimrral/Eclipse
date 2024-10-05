// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Eclipse/GameData/PlayerInventoryStruct.h"
#include "InventoryControllerComponent.generated.h"


enum class EDragOperationType
{
	Inventory,
	GearSlot,
	Ground,
	DeadBody,
	Invalid
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ECLIPSE_API UInventoryControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryControllerComponent();

	// 인벤토리 아이템 드래그 & 드랍 처리 함수
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void HandleDragAndDrop(int32 DraggedIndex, int32 DroppedIndex);

	static EDragOperationType GetDragOperationType(int32 DraggedIndex);

	UPROPERTY()
	FPlayerInventoryStruct PlayerInventoryStruct;

	UPROPERTY()
	class AEclipsePlayerController* OwningController;

	UPROPERTY()
	bool IsAmmunition = false;

	UPROPERTY()
	class AEclipsePlayerState* InventoryManager;

	inline static int32 MinInventoryDragIndexRange = 0;
	inline static int32 MaxInventoryDragIndexRange = 29;
	inline static int32 MinGearSlotDragIndexRange = 38;
	inline static int32 MaxGearSlotDragIndexRange = 42;
	inline static int32 MinGroundDragIndexRange = 46;
	inline static int32 MaxGroundDragIndexRange = 80;
	inline static int32 MinDeadBodyDragIndexRange = 81;
	inline static int32 MaxDeadBodyDragIndexRange = 115;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
