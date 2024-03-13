// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EclipseGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UEclipseGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UEclipseGameInstance();
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsWidgetOn = false;
	
};
