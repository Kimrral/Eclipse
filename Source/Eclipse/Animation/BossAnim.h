// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BossAnim.generated.h"

DECLARE_DELEGATE(FOnMontageSectionFinished);
/**
 * 
 */
UCLASS()
class ECLIPSE_API UBossAnim : public UAnimInstance
{
	GENERATED_BODY()


public:
	UFUNCTION()
	void AnimNotify_MontageEnd() const;

	UFUNCTION()
	void AnimNotify_GroundSmashHitPoint() const;

	UFUNCTION()
	void AnimNotify_UltimateHitPoint() const;

	FOnMontageSectionFinished MontageSectionFinishedDelegate;
	
};
