// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_RandomBehavior.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTD_RandomBehavior : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_RandomBehavior();
	
	UPROPERTY(EditAnywhere)
	float TargetRandomValue;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
