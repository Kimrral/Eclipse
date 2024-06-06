// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "BTS_Detection.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UBTS_Detection : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_Detection();

	UPROPERTY(EditAnywhere)
	float DetectionRange;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
