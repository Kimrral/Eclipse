// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/EclipseAIController.h"

#include "EnemyFSM.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Eclipse/Enemy/Enemy.h"


void AEclipseAIController::OnPossess(APawn* InPawn)
{
	// 빙의 시 호출되는 함수
	Super::OnPossess(InPawn);  // 부모 클래스의 OnPossess 호출

	// InPawn이 AEnemy 타입인지 검사
	if (const auto Enemy = Cast<AEnemy>(InPawn))
	{
		// Enemy 객체가 이동 가능하다면
		if (Enemy->IsRandomMovable)
		{
			// 반복 타이머를 설정하여 일정 간격으로 RandomMove 함수를 호출
			GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AEclipseAIController::RandomMove, RepeatInterval, true);
		}
	}
}

void AEclipseAIController::OnUnPossess()
{
	// 빙의 해제 시 호출되는 함수
	Super::OnUnPossess();  // 부모 클래스의 OnUnPossess 호출

	// 타이머를 클리어하여 RandomMove 함수 호출 중지
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);
}

void AEclipseAIController::RandomMove()
{
	// 현재 AI 컨트롤러가 제어하는 폰이 AEnemy 타입인지 검사
	if (const auto Enemy = Cast<AEnemy>(GetPawn()))
	{
		// Enemy 객체가 유효한지 검사
		if(::IsValid(Enemy))
		{
			// 적의 상태가 IDLE일 때만 랜덤 이동 수행
			if (Enemy->EnemyFSM->State == EEnemyState::IDLE)
			{
				// 월드의 내비게이션 시스템을 가져온다
				const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());

				// 내비게이션 시스템이 유효하지 않으면 함수 종료
				if (NavSystem == nullptr)
					return;

				// 적의 현재 위치를 기준으로 반지름 1500 유닛 내에서 랜덤한 위치를 계산
				if (FNavLocation RandomLocation; NavSystem->GetRandomPointInNavigableRadius(Enemy->GetActorLocation(), 1500.f, RandomLocation))
				{
					// 계산된 랜덤 위치로 이동 명령 발송
					UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomLocation);
					// 랜덤 이동 위치에 대한 델리게이트 호출
					AIControllerRandMoveDelegate.ExecuteIfBound(RandomLocation);
				}
			}
		}		
	}
}

void AEclipseAIController::MoveToPlayer(const AActor* TargetPlayer)
{
	// 타겟 플레이어에게 이동 명령 발송
	UAIBlueprintHelperLibrary::SimpleMoveToActor(this, TargetPlayer);
}

void AEclipseAIController::MoveToLocation(const FVector& TargetLocation)
{
	// 타겟 위치로 이동 명령 발송
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TargetLocation);
}
