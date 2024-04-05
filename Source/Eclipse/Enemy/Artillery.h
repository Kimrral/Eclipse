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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class USkeletalMeshComponent* LauncherComp;
};
