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
	UPROPERTY()
	TArray<class AActor*> outActors;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayerStart> playerStartFactory;
	
	 UFUNCTION()
	 void Respawn(APlayerCharacter* me);

	UFUNCTION()
	void PlayerDeath();
	UFUNCTION()
	void UpdateTabWidget();

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

protected:
	virtual void BeginPlay() override;
};
