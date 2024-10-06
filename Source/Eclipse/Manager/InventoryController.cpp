// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryController.h"

#include "Eclipse/Game/EclipsePlayerState.h"
#include "Eclipse/Player/EclipsePlayerController.h"
#include "Eclipse/UI/InventoryWidget.h"

// Sets default values for this component's properties
UInventoryControllerComponent::UInventoryControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UInventoryControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningController = Cast<AEclipsePlayerController>(GetOwner());
	if (IsValid(OwningController))
	{
		InventoryManager = Cast<AEclipsePlayerState>(OwningController->PlayerCharacter->GetPlayerState());
	}
}


EDragOperationType UInventoryControllerComponent::GetDragOperationType(const int32 DraggedIndex)
{
	if (DraggedIndex >= MinInventoryDragIndexRange && DraggedIndex <= MaxInventoryDragIndexRange)
	{
		return EDragOperationType::Inventory;
	}
	else if (DraggedIndex >= MinGearSlotDragIndexRange && DraggedIndex <= MaxGearSlotDragIndexRange)
	{
		return EDragOperationType::GearSlot;
	}
	else if (DraggedIndex >= MinGroundDragIndexRange && DraggedIndex <= MaxGroundDragIndexRange)
	{
		return EDragOperationType::Ground;
	}
	else if (DraggedIndex >= MinDeadBodyDragIndexRange && DraggedIndex <= MaxDeadBodyDragIndexRange)
	{
		return EDragOperationType::DeadBody;
	}

	return EDragOperationType::Invalid;
}


void UInventoryControllerComponent::HandleDragAndDrop(const int32 DraggedIndex, const int32 DroppedIndex)
{
	if (!IsValid(OwningController))
	{
		return;
	}

	// DraggedIndex 범위에 따른 DragOperationType을 지정
	EDragOperationType OperationType = GetDragOperationType(DraggedIndex);

	// OperationType에 따른 switch-case 처리
	switch (OperationType)
	{
	case EDragOperationType::Inventory:
		InventoryManager->DragFromInventory(OwningController->PlayerCharacter, DraggedIndex, DroppedIndex);
		break;

	case EDragOperationType::GearSlot:
		InventoryManager->DragFromGearSlot(OwningController->PlayerCharacter, DraggedIndex, DroppedIndex);
		break;

	case EDragOperationType::Ground:
		InventoryManager->DragFromGround(OwningController->PlayerCharacter, PlayerInventoryStructs, DroppedIndex, IsAmmunition);
		break;

	case EDragOperationType::DeadBody:
		InventoryManager->DragFromDeadBody(OwningController->PlayerCharacter, DraggedIndex, DroppedIndex);
		break;

	case EDragOperationType::Invalid:
	default:
		UE_LOG(LogTemp, Warning, TEXT("DraggedIndex (%d)는 유효하지 않은 범위입니다."), DraggedIndex);
		break;
	}
}


void UInventoryControllerComponent::AddToInventoryLocal(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct) const
{
	if(IsValid(OwningController))
	{
		OwningController->InventoryWidget->AddToInventoryLocal(PlayerCharacterRef, PlayerInventoryStruct);
	}
}

void UInventoryControllerComponent::SyncInventoryDataFromServer(const TArray<FPlayerInventoryStruct>& ServerInventoryData) const
{
	if(IsValid(OwningController))
	{
		OwningController->InventoryWidget->SyncInventoryDataFromServer(ServerInventoryData);
	}	
}
