// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Manager/InventoryOverlayController.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/UI/InventoryWidget.h"

UInventoryOverlayController::UInventoryOverlayController()
{
}


void UInventoryOverlayController::BeginPlay()
{
	Super::BeginPlay();

	// Assuming PlayerCharacter has the UPlayerCharacterStatComponent attached
	if (const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		PlayerStatComponent = PlayerCharacter->FindComponentByClass<UPlayerCharacterStatComponent>();
	}

	// Initialize InventoryWidget from a reference (could be set via a UPROPERTY or during widget creation)
	// Assuming InventoryWidget has been assigned elsewhere
}

void UInventoryOverlayController::DisplayOverlayInfo() const
{
	if (PlayerStatComponent && InventoryWidget)
	{
		// Update various stats in the overlay
		UpdateHpInfo();
	}
}

void UInventoryOverlayController::UpdateHpInfo() const
{
	if (PlayerStatComponent && InventoryWidget)
	{
		const float CurrentHp = PlayerStatComponent->GetCurrentHp();
		const float MaxHp = PlayerStatComponent->GetMaxHp();

		// Assuming InventoryWidget has a function to update HP info
		InventoryWidget->UpdateHpDisplay(CurrentHp, MaxHp);
	}
}
