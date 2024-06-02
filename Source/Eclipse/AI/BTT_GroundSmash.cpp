// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_GroundSmash.h"

#include "AIController.h"
#include "Eclipse/Animation/BossAnim.h"
#include "Eclipse/Enemy/Boss.h"

UBTT_GroundSmash::UBTT_GroundSmash()
{
}

EBTNodeResult::Type UBTT_GroundSmash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn(); ::IsValid(ControllingPawn))
	{
		if (ABoss* ControllingBoss = Cast<ABoss>(ControllingPawn))
		{
			if (::IsValid(ControllingBoss))
			{
				ControllingBoss->PlayAnimMontageBySectionName(FName("GroundSmash"));
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
