// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTS_CheckChaseLimit.h"

#include "AIController.h"
#include "EclipseAI.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_CheckChaseLimit::UBTS_CheckChaseLimit()
{
	Interval = 2.0f;
}

void UBTS_CheckChaseLimit::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return;
	}
	
	if (const UWorld* World = ControllingPawn->GetWorld(); nullptr == World)
	{
		return;
	}
	
	const FVector InitialPosition = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_INITIALPOS);
	if(const FVector TargetPosition = ControllingPawn->GetActorLocation(); FVector::Dist(InitialPosition, TargetPosition)>4000.f)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKEY_ESCAPEBOUNDARY, true);
		return;
	}
	
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKEY_ESCAPEBOUNDARY, false);
}
