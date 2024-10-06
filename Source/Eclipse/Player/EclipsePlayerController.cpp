// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipsePlayerController.h"

#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Manager/EnemyStatControllerComponent.h"
#include "Eclipse/Manager/InventoryControllerComponent.h"
#include "Eclipse/Manager/PlayerStatControllerComponent.h"

AEclipsePlayerController::AEclipsePlayerController()
{
	InventoryController = CreateDefaultSubobject<UInventoryControllerComponent>(TEXT("InventoryController"));
	EnemyStatController = CreateDefaultSubobject<UEnemyStatControllerComponent>(TEXT("EnemyStatController"));
	PlayerStatController = CreateDefaultSubobject<UPlayerStatControllerComponent>(TEXT("PlayerStatController"));
}


void AEclipsePlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());	
}

