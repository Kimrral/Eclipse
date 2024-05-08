// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Guardian.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AGuardian : public AEnemy
{
	GENERATED_BODY()

public:
	virtual void DropRewardServer() override;
	virtual void FireProcess() const override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGuardianProjectile> GuardianProjectileFactory;
};
