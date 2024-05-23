// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Artillery.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AArtillery : public AEnemy
{
	GENERATED_BODY()
	
public:
	AArtillery();

	virtual void BeginPlay() override;

	virtual void OnDestroy() override;

	virtual void SetDissolveValue(float Value) override;

	virtual void FireProcess() const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class USkeletalMeshComponent* LauncherComp;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGuardianProjectile> GuardianProjectileFactory;
};
