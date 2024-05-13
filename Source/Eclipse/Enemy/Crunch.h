// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Crunch.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API ACrunch : public AEnemy
{
	GENERATED_BODY()

public:
	virtual void SetDissolveMaterial() override;
	
	virtual void SetDissolveValue(float Value) override;
	

};
