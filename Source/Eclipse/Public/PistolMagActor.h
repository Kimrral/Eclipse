// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MagActor.h"
#include "PistolMagActor.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API APistolMagActor : public AMagActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void AddInventory();
	
};
