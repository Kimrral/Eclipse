// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_Return.h"

#include "EclipseAI.h"
#include "EclipseBossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_Return::UBTT_Return()
{
}

EBTNodeResult::Type UBTT_Return::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	const FVector InitialPosition = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_INITIALPOS);
	if(const FVector PawnPosition = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(); FVector::Dist(InitialPosition, PawnPosition)<10.f)
	{
		return EBTNodeResult::Failed;
	}
	
	if (const auto BossController = Cast<AEclipseBossAIController>(OwnerComp.GetAIOwner()))
	{
		if (::IsValid(BossController))
		{			
			BossController->MoveToInitialPosition(InitialPosition);
			BossController->ReturnMovementSuccessDelegate.BindLambda(
				[&]()
				{
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				}
			);		
		}
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}
