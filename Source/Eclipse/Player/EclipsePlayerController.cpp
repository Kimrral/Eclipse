// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipsePlayerController.h"

#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/Enemy/Boss.h"
#include "Eclipse/Game/EclipsePlayerState.h"
#include "Eclipse/Manager/EnemyHpWidgetControllerComponent.h"
#include "Eclipse/Manager/InventoryControllerComponent.h"
#include "Eclipse/UI/BossHPWidget.h"
#include "Kismet/GameplayStatics.h"

AEclipsePlayerController::AEclipsePlayerController()
{
	InventoryController = CreateDefaultSubobject<UInventoryControllerComponent>(TEXT("InventoryController"));
	EnemyHpWidgetController = CreateDefaultSubobject<UEnemyHpWidgetControllerComponent>(TEXT("EnemyHpWidgetController"));
}


void AEclipsePlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	if (::IsValid(PlayerCharacter))
	{
		PlayerCharacter->Stat->OnHpChanged.AddUObject(this, &AEclipsePlayerController::UpdateTabWidget);
		PlayerCharacter->Stat->OnHpZero.AddUObject(this, &AEclipsePlayerController::PlayerDeath);
	}
}

void AEclipsePlayerController::UpdateTabWidget() const
{
	if (PlayerCharacter)
	{
		PlayerCharacter->UpdateTabWidgetHP();
	}
}


void AEclipsePlayerController::PlayerDeath() const
{
	if (PlayerCharacter)
	{
		PlayerCharacter->PlayerDeathRPCServer();
	}
}
