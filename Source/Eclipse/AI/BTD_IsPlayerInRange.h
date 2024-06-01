// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIModule/Classes/BehaviorTree/BTDecorator.h"
#include "BTD_IsPlayerInRange.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTD_IsPlayerInRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTD_IsPlayerInRange();

	UPROPERTY(EditAnywhere)
	float CheckRadius;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
