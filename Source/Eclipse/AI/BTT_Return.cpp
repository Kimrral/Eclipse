// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_Return.h"

#include "EclipseBossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_Return::UBTT_Return()
{
}

EBTNodeResult::Type UBTT_Return::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (const auto BossController = Cast<AEclipseBossAIController>(OwnerComp.GetAIOwner()))
	{
		if (::IsValid(BossController))
		{			
			BossController->BossInitialize();
			return EBTNodeResult::Succeeded;
		}
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
