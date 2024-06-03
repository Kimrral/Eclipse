// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_DashAttack.h"

#include "AIController.h"
#include "Eclipse/Animation/BossAnim.h"
#include "Eclipse/Enemy/Boss.h"

UBTT_DashAttack::UBTT_DashAttack()
{
}

EBTNodeResult::Type UBTT_DashAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn(); ::IsValid(ControllingPawn))
	{
		if (ABoss* ControllingBoss = Cast<ABoss>(ControllingPawn))
		{
			if (::IsValid(ControllingBoss))
			{
				const FName& SectionName = FName("Dash");
				ControllingBoss->PlayAnimMontageBySectionName(SectionName);
				ControllingBoss->LaunchBossCharacter();
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
