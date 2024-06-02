// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTS_CheckHealthPoint.h"

#include "EclipseAI.h"
#include "AIController.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Eclipse/Enemy/Boss.h"

UBTS_CheckHealthPoint::UBTS_CheckHealthPoint()
{
	NodeName = TEXT("CheckHealthPoint");
	Interval = 1.0f;
}

void UBTS_CheckHealthPoint::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if(const ABoss* const ControllingCharacter = Cast<ABoss>(OwnerComp.GetAIOwner()->GetPawn()); ::IsValid(ControllingCharacter))
	{
		const float CurrentHealthPointPercent = (ControllingCharacter->EnemyStat->GetCurrentHp() / ControllingCharacter->EnemyStat->GetMaxHp()) * 100.f;
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(BBKEY_BOSSHP, CurrentHealthPointPercent);
		UE_LOG(LogTemp, Warning, TEXT("%f"), CurrentHealthPointPercent)
		return;
	}
	
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(BBKEY_BOSSHP, 0.f);
}
