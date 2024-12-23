// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Shadow.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AShadow : public AEnemy
{
	GENERATED_BODY()
public:
	AShadow();
	
	virtual void BeginPlay() override;

	virtual void SetDissolveValue(float Value) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class USkeletalMeshComponent* WeaponComp;
	
};
