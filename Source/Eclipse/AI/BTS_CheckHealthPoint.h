// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_CheckHealthPoint.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTS_CheckHealthPoint : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_CheckHealthPoint();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
