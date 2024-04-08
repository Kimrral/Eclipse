// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAnim.h"
#include "DroneAnim.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UDroneAnim : public UEnemyAnim
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void AnimNotify_HitPoint() const;
	
};
