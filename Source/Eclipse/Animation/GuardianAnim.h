// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAnim.h"
#include "GuardianAnim.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UGuardianAnim : public UEnemyAnim
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void AnimNotify_Fire() const;

	
};
