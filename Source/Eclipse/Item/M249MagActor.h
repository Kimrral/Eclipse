// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickableActor.h"
#include "M249MagActor.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AM249MagActor : public APickableActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void AddInventory(APlayerCharacter* CachingPlayerCharacter);
	
};
