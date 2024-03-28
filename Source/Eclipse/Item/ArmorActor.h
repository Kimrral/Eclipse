// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearActor.h"
#include "ArmorActor.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AArmorActor : public AGearActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void AddInventory(APlayerCharacter* CachingPlayerCharacter);
};
