// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Manager/InventoryStatController.h"

#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Player/EclipsePlayerController.h"

UInventoryStatController::UInventoryStatController()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryStatController::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(OwningController))
	{
		PlayerStatComponent = OwningController->PlayerCharacter->Stat;
	}
}

float UInventoryStatController::GetCurrentHp() const
{
	if (PlayerStatComponent)
	{
		return PlayerStatComponent->GetCurrentHp();
	}
	return 0.0f;
}

float UInventoryStatController::GetMaxHp() const
{
	if (PlayerStatComponent)
	{
		return PlayerStatComponent->GetMaxHp();
	}
	return 100.0f;
}

float UInventoryStatController::GetCurrentRouble() const
{
	if (PlayerStatComponent)
	{
		return PlayerStatComponent->GetCurrentRouble();
	}
	return 0.0f;
}

