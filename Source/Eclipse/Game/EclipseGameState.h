// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EclipseGameState.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AEclipseGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void HandleBeginPlay() override;
	virtual void OnRep_ReplicatedHasBegunPlay() override;
	
};
