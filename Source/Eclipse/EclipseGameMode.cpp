// Copyright Epic Games, Inc. All Rights Reserved.

#include "EclipseGameMode.h"
#include "EclipseCharacter.h"
#include "UObject/ConstructorHelpers.h"

AEclipseGameMode::AEclipseGameMode()
{
	curRifleAmmo=40;
	curSniperAmmo=5;
	curPistolAmmo=8;
	curM249Ammo=100;
	maxRifleAmmo=80;
	maxSniperAmmo=10;
	maxPistolAmmo=16;
	maxM249Ammo=200;
}
