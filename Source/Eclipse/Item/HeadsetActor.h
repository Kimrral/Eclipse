// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickableActor.h"
#include "HeadsetActor.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AHeadsetActor : public APickableActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void AddInventory(APlayerCharacter* CachingPlayerCharacter);
	
};
