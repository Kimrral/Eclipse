// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearActor.h"
#include "HelmetActor.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AHelmetActor : public AGearActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void AddInventory();
	
};
