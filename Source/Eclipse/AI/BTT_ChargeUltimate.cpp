// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_ChargeUltimate.h"

#include "AIController.h"
#include "Eclipse/Animation/BossAnim.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/Enemy/Boss.h"
#include "Eclipse/UI/BossShieldWidget.h"

UBTT_ChargeUltimate::UBTT_ChargeUltimate()
{
}

EBTNodeResult::Type UBTT_ChargeUltimate::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn(); ::IsValid(ControllingPawn))
	{
		if (ABoss* ControllingBoss = Cast<ABoss>(ControllingPawn))
		{
			if (::IsValid(ControllingBoss))
			{
				const FName& SectionName = FName("ChargeUltimate");
				ControllingBoss->PlayAnimMontageBySectionName(SectionName);
				ControllingBoss->SetBossShieldWidget();
				ControllingBoss->ShieldDestroySuccessDelegate.BindLambda(
					[&]()
					{
						FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					}
				);
				if (const auto BossAnimInstance = Cast<UBossAnim>(ControllingBoss->GetMesh()->GetAnimInstance()); ::IsValid(BossAnimInstance))
				{
					BossAnimInstance->MontageSectionFinishedDelegate.BindLambda(
						[&]()
						{
							FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
						}
					);
				}
			}
		}
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}
