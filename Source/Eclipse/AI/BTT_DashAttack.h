// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_DashAttack.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTT_DashAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_DashAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
