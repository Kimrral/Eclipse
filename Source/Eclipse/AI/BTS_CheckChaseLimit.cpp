// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTS_CheckChaseLimit.h"

#include "AIController.h"
#include "EclipseAI.h"
#include "BehaviorTree/BlackboardComponent.h"

// UBTD_CheckChaseLimit 클래스의 생성자
UBTS_CheckChaseLimit::UBTS_CheckChaseLimit()
{
	// 노드의 실행 간격을 2초로 설정
	Interval = 2.0f;
}

// 비헤이비어 트리 노드의 틱(Tick) 함수
void UBTS_CheckChaseLimit::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds)
{
	// 부모 클래스의 TickNode 함수를 호출
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// AI 컨트롤러가 제어하는 폰을 가져옴
	const APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	// 제어하는 폰이 없으면 함수를 종료
	if (nullptr == ControllingPawn)
	{
		return;
	}
	
	// 폰이 속한 월드를 가져옴
	if (const UWorld* World = ControllingPawn->GetWorld(); nullptr == World)
	{
		return;
	}
	
	// 블랙보드에서 초기 위치 값을 가져옴
	const FVector InitialPosition = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_INITIALPOS);
	// 현재 폰의 위치를 가져옴
	if (const FVector TargetPosition = ControllingPawn->GetActorLocation(); FVector::Dist(InitialPosition, TargetPosition) > 4000.f)
	{
		// 초기 위치와 현재 위치 사이의 거리가 4000.f보다 크면 블랙보드에 ESCAPEBOUNDARY 값을 true로 설정
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKEY_ESCAPEBOUNDARY, true);
		return;
	}
	
	// 그렇지 않으면 블랙보드에 ESCAPEBOUNDARY 값을 false로 설정
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BBKEY_ESCAPEBOUNDARY, false);
}
