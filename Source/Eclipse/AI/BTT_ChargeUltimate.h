// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ChargeUltimate.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTT_ChargeUltimate : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_ChargeUltimate();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
