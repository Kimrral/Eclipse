// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_BasicAttack.h"

#include "AIController.h"
#include "Eclipse/Animation/BossAnim.h"
#include "Eclipse/Enemy/Boss.h"

UBTT_BasicAttack::UBTT_BasicAttack()
{
}

EBTNodeResult::Type UBTT_BasicAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn(); ::IsValid(ControllingPawn))
	{
		if (ABoss* ControllingBoss = Cast<ABoss>(ControllingPawn))
		{
			if (::IsValid(ControllingBoss))
			{
				if (const auto RandIndex = FMath::RandRange(0, 3); RandIndex >= 0 && RandIndex <= 3)
				{
					const FString SectionName = FString::Printf(TEXT("PrimaryAttack%d"), RandIndex + 1);
					ControllingBoss->PlayAnimMontageBySectionName(FName(*SectionName));
				}
				
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
