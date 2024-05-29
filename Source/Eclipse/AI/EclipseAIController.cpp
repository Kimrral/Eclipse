// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/EclipseAIController.h"

#include "EnemyFSM.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Eclipse/Enemy/Enemy.h"

AEclipseAIController::AEclipseAIController()
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


void AEclipseAIController::OnPossess(APawn* InPawn)
{
	// 빙의 시
	Super::OnPossess(InPawn);

	if (const auto Enemy = Cast<AEnemy>(InPawn))
	{
		if (Enemy->IsRandomMovable)
		{
			GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AEclipseAIController::RandomMove, RepeatInterval, true);
		}
	}
}

void AEclipseAIController::OnUnPossess()
{
	// 빙의 해제 시
	Super::OnUnPossess();

	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);
}

void AEclipseAIController::RunAI()
{
	if (UBlackboardComponent* BlackboardPtr = Blackboard.Get(); UseBlackboard(ECBlackboard, BlackboardPtr))
	{
		RunBehaviorTree(ECBehaviorTree);
	}
}

void AEclipseAIController::StopAI() const
{
	if (UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		BehaviorTreeComponent->StopTree();
	}
}

void AEclipseAIController::RandomMove()
{
	if (const auto Enemy = Cast<AEnemy>(GetPawn()))
	{
		if (Enemy->EnemyFSM->State == EEnemyState::IDLE)
		{
			// 월드의 내비게이션 시스템을 가져온다
			const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());

			if (NavSystem == nullptr)
				return;

			// 월드의 내비게이션 범위중 원점(첫번째 매개변수)을 기준으로 반지름 만큼의 범위를 한정지어
			// 랜덤한 좌표를 가져온다(RandomLocation에 저장됨)
			if (FNavLocation RandomLocation; NavSystem->GetRandomPointInNavigableRadius(Enemy->GetActorLocation(), 1500.f, RandomLocation))
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomLocation);
				AIControllerRandMoveDelegate.ExecuteIfBound(RandomLocation);
			}
		}
	}
}

void AEclipseAIController::MoveToPlayer(const AActor* TargetPlayer)
{
	UAIBlueprintHelperLibrary::SimpleMoveToActor(this, TargetPlayer);
}

void AEclipseAIController::MoveToLocation(const FVector& TargetLocation)
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TargetLocation);
}
