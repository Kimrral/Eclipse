// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Manager/InventoryOverlayController.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/UI/InventoryWidget.h"


void UInventoryOverlayController::BeginPlay()
{
	Super::BeginPlay();

	// PlayerCharacter has the UPlayerCharacterStatComponent attached
	if (const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		PlayerStatComponent = PlayerCharacter->FindComponentByClass<UPlayerCharacterStatComponent>();
	}
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

		InventoryWidget->UpdateHpDisplay(CurrentHp, MaxHp);
	}
}
