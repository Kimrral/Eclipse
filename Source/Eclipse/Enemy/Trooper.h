// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Trooper.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API ATrooper : public AEnemy
{
	GENERATED_BODY()
public:
	ATrooper();
	
	virtual void BeginPlay() override;

	virtual void OnDestroy() override;

	virtual void SetDissolveValue(float Value) override;
	
	virtual void FireProcess() const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class USkeletalMeshComponent* WeaponComp;

	UPROPERTY(EditAnywhere)
	class USoundBase* TrooperFireSound;

	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class ATrooperProjectile> TrooperProjectileFactory;
	
};
