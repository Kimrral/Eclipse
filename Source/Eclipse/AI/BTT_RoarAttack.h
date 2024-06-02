// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIModule/Classes/BehaviorTree/BTTaskNode.h"
#include "BTT_RoarAttack.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTT_RoarAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_RoarAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
