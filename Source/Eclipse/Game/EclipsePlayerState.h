// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "EclipsePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AEclipsePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void DeadBodyWidgetSettings(ADeadPlayerContainer*  DeadPlayerContainer, APlayerCharacter* InstigatorPlayerRef);

	virtual  void CopyProperties(APlayerState* PlayerState) override;
	
};
