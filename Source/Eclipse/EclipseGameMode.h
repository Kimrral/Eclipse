// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EclipseGameMode.generated.h"

UCLASS(minimalapi)
class AEclipseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEclipseGameMode();

	UPROPERTY()
	int ConsoleCount;

	UPROPERTY()
	int GuardianCount;

	UPROPERTY()
	int BossCount;

	UPROPERTY()
	int curRifleAmmo;

	UPROPERTY()
	int curSniperAmmo;

	UPROPERTY()
	int curPistolAmmo;

	UPROPERTY()
	int curM249Ammo;

	UPROPERTY()
	int maxRifleAmmo;

	UPROPERTY()
	int maxSniperAmmo;

	UPROPERTY()
	int maxPistolAmmo;

	UPROPERTY()
	int maxM249Ammo;

	
};



