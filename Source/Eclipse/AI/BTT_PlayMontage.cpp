// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_PlayMontage.h"

#include "AIController.h"
#include "Eclipse/Animation/BossAnim.h"
#include "Eclipse/Enemy/Boss.h"

UBTT_PlayMontage::UBTT_PlayMontage()
{
}

EBTNodeResult::Type UBTT_PlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn(); ::IsValid(ControllingPawn))
	{
		if (ABoss* ControllingBoss = Cast<ABoss>(ControllingPawn))
		{
			if (::IsValid(ControllingBoss))
			{
				ControllingBoss->PlayAnimMontageBySectionName(MontageSectionName);
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

	return EBTNodeResult::InProgress;
}
