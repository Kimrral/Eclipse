// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipsePlayerController.h"

#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Game/EclipseGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AEclipsePlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	if (PlayerCharacter)
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


