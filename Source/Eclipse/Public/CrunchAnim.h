// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAnim.h"
#include "CrunchAnim.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UCrunchAnim : public UEnemyAnim
{
	GENERATED_BODY()

public:
    virtual void AnimNotify_AttackStart() override;

	UPROPERTY(EditAnywhere)
	class USoundBase* punchSoundNoHit;
	
};
