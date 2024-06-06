// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIModule/Classes/BehaviorTree/BTService.h"
#include "BTS_CheckChaseLimit.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTS_CheckChaseLimit : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_CheckChaseLimit();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
