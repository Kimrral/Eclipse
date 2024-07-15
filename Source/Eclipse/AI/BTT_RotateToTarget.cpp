// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_RotateToTarget.h"

#include "AIController.h"
#include "EclipseAI.h"
#include "BehaviorTree/BlackboardComponent.h"

// UBTT_RotateToTarget 클래스의 생성자
UBTT_RotateToTarget::UBTT_RotateToTarget()
{
	// 노드 이름을 "Turn"으로 설정
	NodeName = TEXT("Turn");
}

// 비헤이비어 트리 태스크를 실행하는 함수
EBTNodeResult::Type UBTT_RotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 부모 클래스의 ExecuteTask 함수를 호출하고 그 결과를 저장
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI 컨트롤러가 제어하는 폰을 APawn 타입으로 캐스팅
	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	// 제어하는 폰이 유효하지 않으면 태스크 실패를 반환
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 블랙보드에서 타겟 폰을 APawn 타입으로 캐스팅
	const APawn* TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	// 타겟 폰이 유효하지 않으면 태스크 실패를 반환
	if (nullptr == TargetPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 타겟 위치와 제어하는 폰 위치 간의 벡터를 계산
	FVector LookVector = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();
	// Z 축 방향 성분을 0으로 설정 (2D 회전)
	LookVector.Z = 0.0f;
	// LookVector를 기반으로 목표 회전을 계산
	const FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	// 현재 회전을 목표 회전으로 부드럽게 보간하여 회전
	ControllingPawn->SetActorRotation(FMath::RInterpTo(ControllingPawn->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), TurnSpeed));

	// 태스크가 성공적으로 완료되었음을 반환
	return EBTNodeResult::Succeeded;
}
