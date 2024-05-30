// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Boss.generated.h"

UCLASS()
class ECLIPSE_API ABoss : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss();
	
	// Enemy Stat
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyCharacterStatComponent> EnemyStat;
	
	// Reward Manager
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class URewardManagerComponent> RewardManager;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
