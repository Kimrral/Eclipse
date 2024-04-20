// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Spider.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API ASpider : public AEnemy
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	class UAnimMontage* SpiderSpawnMontage;
};
