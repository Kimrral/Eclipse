// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_InitializeRotation.h"

#include "AIController.h"

// UBTT_InitializeRotation 클래스의 생성자
UBTT_InitializeRotation::UBTT_InitializeRotation()
{
}

// 비헤이비어 트리 태스크를 실행하는 함수
EBTNodeResult::Type UBTT_InitializeRotation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	// 목표로 하는 회전값을 설정 (피치 0, 요 -50, 롤 0)
	const FRotator LookRotator = FRotator(0, -50, 0);
	// 현재 회전을 목표 회전으로 부드럽게 보간하여 회전
	ControllingPawn->SetActorRotation(FMath::RInterpTo(ControllingPawn->GetActorRotation(), LookRotator, GetWorld()->GetDeltaSeconds(), TurnSpeed));

	// 태스크가 성공적으로 완료되었음을 반환
	return EBTNodeResult::Succeeded;
}
