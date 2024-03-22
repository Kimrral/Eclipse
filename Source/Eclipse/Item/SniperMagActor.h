// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MagActor.h"
#include "SniperMagActor.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API ASniperMagActor : public AMagActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void AddInventory();
	
};
