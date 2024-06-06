// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EclipseBossAIController.generated.h"

DECLARE_DELEGATE(FOnReturnMovementSuccess);
/**
 * 
 */
UCLASS()
class ECLIPSE_API AEclipseBossAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEclipseBossAIController();
	void RunAI();
	void StopAI() const;

	UFUNCTION()
	void BossInitialize() const;

	FOnReturnMovementSuccess ReturnMovementSuccessDelegate;
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBlackboardData> ECBlackboard;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBehaviorTree> ECBehaviorTree;
	
};
