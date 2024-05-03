// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/GameData/PlayerInventoryStruct.h"
#include "GameFramework/PlayerState.h"
#include "EclipsePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AEclipsePlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AEclipsePlayerState();
	
protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void DeadBodyWidgetSettings(ADeadPlayerContainer* DeadPlayerContainer, APlayerCharacter* InstigatorPlayerRef);

	UFUNCTION(Server, Reliable, WithValidation)
	void DeadBodyWidgetSettingsServer(ADeadPlayerContainer* DeadPlayerContainer, APlayerCharacter* InstigatorPlayerRef);

	UFUNCTION(NetMulticast, Reliable)
	void DeadBodyWidgetSettingsMulticast(ADeadPlayerContainer* DeadPlayerContainer, APlayerCharacter* InstigatorPlayerRef);

	UFUNCTION(BlueprintImplementableEvent)
	void DeadBodySettingsOnWidgetClass(APlayerCharacter* PlayerCharacterRef, const TArray<FPlayerInventoryStruct> &DeadPlayerInventoryArrayRef, const TArray<int32> &DeadPlayerStackArrayRef, const TArray<FPlayerInventoryStruct> &DeadPlayerGearArrayRef);	

	UFUNCTION(BlueprintImplementableEvent)
	void AddToInventoryWidget(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct);

	UFUNCTION(BlueprintCallable)
	void AddToInventory(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct);

	UFUNCTION(Server, Reliable, WithValidation)
	void AddToInventoryServer(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct);

	UFUNCTION(NetMulticast, Reliable)
	void AddToInventoryMulticast(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct);

	UFUNCTION(BlueprintCallable)
	void OnUseConsumableItem(APlayerCharacter* PlayerCharacterRef, const FString& ConsumableItemName, float HealAmount);

	UFUNCTION(Server, Reliable, WithValidation)
	void OnUseConsumableItemServer(APlayerCharacter* PlayerCharacterRef, const FString& ConsumableItemName, const float HealAmount);

	UFUNCTION(BlueprintCallable)
	void DragFromDeadBody(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void DragFromDeadBodyServer(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex);

	UFUNCTION(NetMulticast, Reliable)
	void DragFromDeadBodyMulticast(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex);

	UFUNCTION(BlueprintCallable)
	void DragFromGearSlot(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void DragFromGearSlotServer(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex);

	UFUNCTION(NetMulticast, Reliable)
	void DragFromGearSlotMulticast(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex);

	UFUNCTION(BlueprintCallable)
	void DragFromGround(APlayerCharacter* PlayerCharacterRef,  const FPlayerInventoryStruct& PlayerInventoryStruct, const int32 DropArrayIndex, const bool IsAmmunition);

	UFUNCTION(Server, Reliable, WithValidation)
	void DragFromGroundServer(APlayerCharacter* PlayerCharacterRef,  const FPlayerInventoryStruct& PlayerInventoryStruct, const int32 DropArrayIndex, const bool IsAmmunition);

	UFUNCTION(NetMulticast, Reliable)
	void DragFromGroundMulticast(APlayerCharacter* PlayerCharacterRef,  const FPlayerInventoryStruct& PlayerInventoryStruct, const int32 DropArrayIndex, const bool IsAmmunition);

	UFUNCTION(BlueprintCallable)
	void DragFromInventory(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void DragFromInventoryServer(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex);

	UFUNCTION(NetMulticast, Reliable)
	void DragFromInventoryMulticast(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void GroundDetectAndDestroy(FVector PlayerCharacterLocation, const FString &InputItemString);

	UFUNCTION(BlueprintCallable)
	void RemoveSoldInventoryIndex(APlayerCharacter* PlayerCharacterRef, const TArray<int32>& SoldInventoryIndexArray, const int32 SoldRoubleAmount);

	UFUNCTION(Server, Reliable, WithValidation)
	void RemoveSoldInventoryIndexServer(APlayerCharacter* PlayerCharacterRef, const TArray<int32>& SoldInventoryIndexArray, const int32 SoldRoubleAmount);

	UFUNCTION(BlueprintCallable)
	void ModifyRouble(APlayerCharacter* PlayerCharacterRef, const float RoubleAmount);

	UFUNCTION(Server, Reliable, WithValidation)
	void ModifyRoubleServer(APlayerCharacter* PlayerCharacterRef, const float RoubleAmount);

	UFUNCTION()
	void MoveInventoryDataToGameInstance() const;

	UFUNCTION()
	void GetInventoryDataFromGameInstance();

	UFUNCTION()
	void ApplyGearInventoryEquipState(APlayerCharacter* PlayerCharacterRef);

	virtual void CopyProperties(APlayerState* PlayerState) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	TArray<FPlayerInventoryStruct> PlayerInventoryStructs;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	TArray<int32> PlayerInventoryStacks;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	TArray<FPlayerInventoryStruct> PlayerGearSlotStructs;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	TArray<FPlayerInventoryStruct> DeadPlayerInventoryStructs;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	TArray<int32> DeadPlayerInventoryStacks;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	TArray<FPlayerInventoryStruct> DeadPlayerGearSlotStructs;

	UPROPERTY()
	FPlayerInventoryStruct InventoryStructDefault;
	
	UPROPERTY()
	FPlayerInventoryStruct InventoryDropStructCache;

	UPROPERTY()
	int32 InventoryDropStackCache;

	UPROPERTY()
	bool IsAlreadySet = false;

	UPROPERTY()
	bool IsAlreadyAccessed = false;
};
