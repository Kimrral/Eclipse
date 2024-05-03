// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipseGameInstance.h"

UEclipseGameInstance::UEclipseGameInstance()
{
	PlayerInventoryStructs.Init(InventoryStructDefault, 30);
	PlayerInventoryStacks.Init(0, 30);
	PlayerGearSlotStructs.Init(InventoryStructDefault, 5);

	curRifleAmmo = 40;
	curSniperAmmo = 5;
	curPistolAmmo = 8;
	curM249Ammo = 100;
	maxRifleAmmo = 40;
	maxSniperAmmo = 5;
	maxPistolAmmo = 8;
	maxM249Ammo = 100;
}
