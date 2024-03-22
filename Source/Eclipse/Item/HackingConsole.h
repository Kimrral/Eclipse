// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RewardActor.h"
#include "HackingConsole.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AHackingConsole : public ARewardActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void AddInventory();
		
};
