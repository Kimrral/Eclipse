// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	
	UPROPERTY()
	class APlayerCharacter* me;

	UFUNCTION()
	void AnimNotify_ReloadStart();

	UFUNCTION()
	void AnimNotify_ReloadEnd();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPistol = false;
};
