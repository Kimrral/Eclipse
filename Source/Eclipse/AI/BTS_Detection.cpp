// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTS_Detection.h"

#include "AIController.h"
#include "EclipseAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Eclipse/Character/PlayerCharacter.h"

UBTS_Detection::UBTS_Detection()
{
	NodeName = TEXT("Detection");
	Interval = 1.0f;
}

void UBTS_Detection::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return;
	}

	const FVector Center = ControllingPawn->GetActorLocation();
	const UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World)
	{
		return;
	}	

	TArray<FOverlapResult> OverlapResults;
	const FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, ControllingPawn);

	if (bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(DetectionRange),
		CollisionQueryParam
	))
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(OverlapResult.GetActor()); ::IsValid(Player))
			{
				if(Player->IsPlayerDeadImmediately)
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, nullptr);				
					return;
				}
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, Player);				
				return;
			}
		}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, nullptr);
}
