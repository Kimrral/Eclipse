// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTD_IsPlayerInRange.h"

#include "AIController.h"
#include "EclipseAI.h"
#include "BehaviorTree/BlackboardComponent.h"

// UBTD_IsPlayerInRange 클래스의 생성자
UBTD_IsPlayerInRange::UBTD_IsPlayerInRange()
{
	// 이 비헤이비어 트리 노드의 이름을 설정
	NodeName = TEXT("IsInRange");
}

// 비헤이비어 트리 노드의 조건 값을 계산하는 함수
bool UBTD_IsPlayerInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	// 부모 클래스의 CalculateRawConditionValue 함수를 호출하고 그 결과를 저장
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	// AI 컨트롤러가 제어하는 폰을 가져옴
	const APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	// 제어하는 폰이 없으면 false 반환
	if (nullptr == ControllingPawn)
	{
		return false;
	}

	// 블랙보드에서 타겟 폰을 가져옴
	const APawn* Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	// 타겟 폰이 없으면 false 반환
	if (nullptr == Target)
	{
		return false;
	}

	// 제어하는 폰과 타겟 폰 사이의 거리를 계산
	const float DistanceToTarget = ControllingPawn->GetDistanceTo(Target);
	// 거리가 설정된 반경(CheckRadius) 이내인지 확인하고 결과를 bResult에 저장
	bResult = (DistanceToTarget <= CheckRadius);
	// 최종 결과를 반환
	return bResult;
}

