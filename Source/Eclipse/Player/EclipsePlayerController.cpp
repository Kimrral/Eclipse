// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipsePlayerController.h"

#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Manager/EnemyStatController.h"
#include "Eclipse/Manager/InventoryController.h"
#include "Eclipse/Manager/PlayerStatController.h"
#include "Eclipse/Manager/InventoryOverlayController.h"
#include "Eclipse/Manager/InventoryStatController.h"

AEclipsePlayerController::AEclipsePlayerController()
{
	InventoryController = CreateDefaultSubobject<UInventoryControllerComponent>(TEXT("InventoryController"));
	InventoryStatController = CreateDefaultSubobject<UInventoryStatController>(TEXT("InventoryStatController"));
	InventoryOverlayController = CreateDefaultSubobject<UInventoryOverlayController>(TEXT("InventoryOverlayController"));
	EnemyStatController = CreateDefaultSubobject<UEnemyStatController>(TEXT("EnemyStatController"));
	PlayerStatController = CreateDefaultSubobject<UPlayerStatController>(TEXT("PlayerStatController"));
}

void AEclipsePlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
}
