// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Return.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTT_Return : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_Return();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
