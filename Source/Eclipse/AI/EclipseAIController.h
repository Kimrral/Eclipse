// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EclipseAIController.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRandomMoveBegin, FVector, RandomLocation);
/**
 * 
 */
UCLASS()
class ECLIPSE_API AEclipseAIController : public AAIController
{
	GENERATED_BODY()

public:	
	void RandomMove();
	void MoveToPlayer(const AActor* TargetPlayer);
	void MoveToLocation(const FVector& TargetLocation);

	FOnRandomMoveBegin AIControllerRandMoveDelegate;

	FTimerHandle RepeatTimerHandle;
	float RepeatInterval = 15.0f;

	UPROPERTY(EditAnywhere, Category=Widget)
	TSubclassOf<class UBossHPWidget> BossHPWidgetFactory;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBossHPWidget> BossHPWidget;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

};
