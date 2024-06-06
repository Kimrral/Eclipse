// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_InitializeRotation.h"

#include "AIController.h"

UBTT_InitializeRotation::UBTT_InitializeRotation()
{
}

EBTNodeResult::Type UBTT_InitializeRotation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}


	const FRotator LookRotator = FRotator(0, -90, 0);
	//const FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	ControllingPawn->SetActorRotation(FMath::RInterpTo(ControllingPawn->GetActorRotation(), LookRotator, GetWorld()->GetDeltaSeconds(), TurnSpeed));

	return EBTNodeResult::Succeeded;
}
