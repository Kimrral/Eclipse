// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_PlayMontage.h"

#include "AIController.h"
#include "Eclipse/Enemy/Boss.h"

UBTT_PlayMontage::UBTT_PlayMontage()
{
}

EBTNodeResult::Type UBTT_PlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if(const APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn(); !::IsValid(ControllingPawn))
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		if(const ABoss* ControllingBoss = Cast<ABoss>(ControllingPawn))
		{
			if(const auto BossAnimInstance = ControllingBoss->GetMesh()->GetAnimInstance(); ::IsValid(BossAnimInstance))
			{
				if (const float Duration = BossAnimInstance->Montage_Play(ControllingBoss->StunMontage); Duration > 0.f)
				{
					// Start at a given Section.
					if( MontageSectionName != NAME_None )
					{
						BossAnimInstance->Montage_JumpToSection(MontageSectionName, ControllingBoss->StunMontage);
						UE_LOG(LogTemp, Warning, TEXT("BTMontage"))
						return EBTNodeResult::Succeeded;
					}
				}
			}
		}
	}
	
	return EBTNodeResult::Failed;
}
