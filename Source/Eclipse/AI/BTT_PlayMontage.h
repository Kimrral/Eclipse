// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIModule/Classes/BehaviorTree/BTTaskNode.h"
#include "BTT_PlayMontage.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTT_PlayMontage : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_PlayMontage();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	FName MontageSectionName;
	
};
