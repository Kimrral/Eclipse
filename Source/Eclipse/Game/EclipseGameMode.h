// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "EclipseGameMode.generated.h"

UCLASS(minimalapi)
class AEclipseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEclipseGameMode();

	UFUNCTION()
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void OnPlayerKilled(AController* Killer, AController* KilledPlayer, APawn* KilledPawn);

	UPROPERTY()
	TArray<class AActor*> outActors;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayerStart> playerStartFactory;
};
