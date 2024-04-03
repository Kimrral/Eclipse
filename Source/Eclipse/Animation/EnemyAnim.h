// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Eclipse/AI/EnemyFSM.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnim.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;

	UFUNCTION()
	virtual void AnimNotify_AttackStart();

	UFUNCTION()
	virtual void AnimNotify_AttackEnd();

	UFUNCTION()
	virtual void AnimNotify_DamageEnd();

	UFUNCTION()
	virtual void AnimNotify_DieEnd();

	UFUNCTION()
	void ReplicateChangedState();

	UPROPERTY()
	class AEnemy* me;

	UPROPERTY()
	UEnemyFSM* FSM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemyState state;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsPlayingAttackAnimation = false;;

};
