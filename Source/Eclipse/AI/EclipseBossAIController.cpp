// Fill out your copyright notice in the Description page of Project Settings.

#include "Eclipse/AI/EclipseBossAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "EclipseAI.h"

AEclipseBossAIController::AEclipseBossAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BlackBoardAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/KHJContent/AI/BB_Enemy.BB_Enemy'"));
	if (nullptr != BlackBoardAssetRef.Object)
	{
		ECBlackboard = BlackBoardAssetRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/KHJContent/AI/BT_Enemy.BT_Enemy'"));
	if (nullptr != BehaviorTreeAssetRef.Object)
	{
		ECBehaviorTree = BehaviorTreeAssetRef.Object;
	}
}

void AEclipseBossAIController::RunAI()
{
	if (UBlackboardComponent* BlackboardPtr = Blackboard.Get(); UseBlackboard(ECBlackboard, BlackboardPtr))
	{
		Blackboard->SetValueAsVector(BBKEY_INITIALPOS, GetPawn()->GetActorLocation());
		
		RunBehaviorTree(ECBehaviorTree);
	}
}

void AEclipseBossAIController::StopAI() const
{
	if (UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		BehaviorTreeComponent->StopTree();
	}
}

void AEclipseBossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();
}

void AEclipseBossAIController::OnUnPossess()
{
	Super::OnUnPossess();

	StopAI();
}
