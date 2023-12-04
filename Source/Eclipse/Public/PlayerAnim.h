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

	UFUNCTION()
	void AnimNotify_LeftPlant();

	UFUNCTION()
	void AnimNotify_RightPlant();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPistol = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bZooming = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRifleZooming = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bM249Zooming = false;
	
	UPROPERTY(EditAnywhere, Category=Sound)
	class USoundBase* walkSound;
};
