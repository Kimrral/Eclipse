// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsumableActor.h"
#include "MedKitActor.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AMedKitActor : public AConsumableActor
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void AddInventory();
	
};
