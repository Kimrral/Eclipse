// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAnim.h"
#include "ArtilleryAnim.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UArtilleryAnim : public UEnemyAnim
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void AnimNotify_Fire() const;

};
