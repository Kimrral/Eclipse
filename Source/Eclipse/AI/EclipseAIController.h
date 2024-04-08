// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EclipseAIController.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AEclipseAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	void RandomMove();
	void MoveToPlayer(const AActor* TargetPlayer);
	void MoveToLocation(const FVector& TargetLocation);

	FTimerHandle RepeatTimerHandle;
	float RepeatInterval = 15.0f;
};
