// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTS_CheckHealthPoint.h"

#include "EclipseAI.h"
#include "AIController.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Eclipse/Enemy/Boss.h"

// UBTD_CheckHealthPoint 클래스의 생성자
UBTS_CheckHealthPoint::UBTS_CheckHealthPoint()
{
	// 이 비헤이비어 트리 노드의 이름을 설정
	NodeName = TEXT("CheckHealthPoint");
	// 노드의 실행 간격을 1초로 설정
	Interval = 1.0f;
}

// 비헤이비어 트리 노드의 틱(Tick) 함수
void UBTS_CheckHealthPoint::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds)
{
	// 부모 클래스의 TickNode 함수를 호출
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// AI 컨트롤러가 제어하는 폰을 ABoss 타입으로 캐스팅하여 가져옴
	if (const ABoss* const ControllingCharacter = Cast<ABoss>(OwnerComp.GetAIOwner()->GetPawn()); ::IsValid(ControllingCharacter))
	{
		// 현재 체력 백분율을 계산
		const float CurrentHealthPointPercent = (ControllingCharacter->EnemyStat->GetCurrentHp() / ControllingCharacter->EnemyStat->GetMaxHp()) * 100.f;
		// 블랙보드에 현재 체력 백분율을 설정
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(BBKEY_BOSSHP, CurrentHealthPointPercent);
		// 현재 체력 백분율을 로그로 출력
		UE_LOG(LogTemp, Warning, TEXT("%f"), CurrentHealthPointPercent)
		return;
	}

	// 캐릭터가 유효하지 않으면 블랙보드에 체력 값을 0으로 설정
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(BBKEY_BOSSHP, 0.f);
}
