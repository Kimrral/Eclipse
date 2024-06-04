// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTD_RandomBehavior.h"

#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

UBTD_RandomBehavior::UBTD_RandomBehavior()
{
}

bool UBTD_RandomBehavior::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	const APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return false;
	}

	const double RandomFloat = UKismetMathLibrary::RandomFloatInRange(0, 100);
	bResult = (TargetRandomValue<=RandomFloat);
	return bResult;
}
