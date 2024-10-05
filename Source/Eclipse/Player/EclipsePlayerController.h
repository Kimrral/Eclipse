// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "EclipsePlayerController.generated.h"

/**
 * 
 */

enum class EDragOperationType
{
	Inventory,
	GearSlot,
	Ground,
	DeadBody,
	Invalid
};

UCLASS()
class ECLIPSE_API AEclipsePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	void UpdateTabWidget() const;

	UFUNCTION()
	void UpdateBossHpWidget(const float InCurrentHp, const float InMaxHp);

	UFUNCTION()
	void UpdateBossShieldWidget(const float InCurrentShield, const float InMaxShield);

	UFUNCTION()
	void PlayerDeath() const;

	// 인벤토리 아이템 드래그 & 드랍 처리 함수
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void HandleDragAndDrop(int32 DraggedIndex, int32 DroppedIndex);
	
	static EDragOperationType GetDragOperationType(int32 DraggedIndex);

	UPROPERTY()
	FPlayerInventoryStruct PlayerInventoryStruct;

	UPROPERTY()
	bool IsAmmunition = false;

	inline static int32 MinInventoryDragIndexRange = 0;
	inline static int32 MaxInventoryDragIndexRange = 29;
	inline static int32 MinGearSlotDragIndexRange = 38;
	inline static int32 MaxGearSlotDragIndexRange = 42;
	inline static int32 MinGroundDragIndexRange = 46;
	inline static int32 MaxGroundDragIndexRange = 80;
	inline static int32 MinDeadBodyDragIndexRange = 81;
	inline static int32 MaxDeadBodyDragIndexRange = 115;

	UFUNCTION()
	void AddBossHpWidgetToViewport();
	UFUNCTION()
	void RemoveBossHpWidgetFromViewport() const;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayerStart> PlayerStartFactory;	

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class UBossHPWidget> BossHPWidgetFactory;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TObjectPtr<class UBossHPWidget> BossHPWidget;

	UPROPERTY(EditAnywhere)
	class AEclipsePlayerState* InventoryManager;

protected:
	virtual void BeginPlay() override;
};
