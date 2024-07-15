// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTD_RandomBehavior.h"

#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

// UBTD_RandomBehavior 클래스의 생성자
UBTD_RandomBehavior::UBTD_RandomBehavior()
{
}

// 비헤이비어 트리 노드의 조건 값을 계산하는 함수
bool UBTD_RandomBehavior::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
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

	// 0에서 100 사이의 랜덤한 부동 소수점 수를 생성
	const double RandomFloat = UKismetMathLibrary::RandomFloatInRange(0, 100);
	// 랜덤 값이 TargetRandomValue보다 큰지 여부를 bResult에 저장
	bResult = (TargetRandomValue <= RandomFloat);
	// 최종 결과를 반환
	return bResult;
}
