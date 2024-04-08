// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAnim.h"
#include "SpiderAnim.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API USpiderAnim : public UEnemyAnim
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void AnimNotify_HitPoint() const;
	
};
