// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_Return.h"

#include "EclipseBossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// UBTT_Return 클래스의 생성자
UBTT_Return::UBTT_Return()
{
}

// 비헤이비어 트리 태스크를 실행하는 함수
EBTNodeResult::Type UBTT_Return::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 부모 클래스의 ExecuteTask 함수를 호출하고 그 결과를 저장
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI 컨트롤러를 AEclipseBossAIController 타입으로 캐스팅
	if (const auto BossController = Cast<AEclipseBossAIController>(OwnerComp.GetAIOwner()))
	{
		// 캐스팅이 성공하고 보스 컨트롤러가 유효하면
		if (::IsValid(BossController))
		{
			// 보스 컨트롤러의 BossInitialize 함수를 호출
			BossController->BossInitialize();
			// 태스크가 성공적으로 완료되었음을 반환
			return EBTNodeResult::Succeeded;
		}
	}
	// AI 컨트롤러가 AEclipseBossAIController 타입으로 캐스팅되지 않거나 유효하지 않으면 태스크 실패를 반환
	return EBTNodeResult::Failed;
}
