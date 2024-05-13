// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "EclipsePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API AEclipsePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	void UpdateTabWidget() const;

	UFUNCTION()
	void PlayerDeath() const;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayerStart> PlayerStartFactory;	

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

protected:
	virtual void BeginPlay() override;
};
