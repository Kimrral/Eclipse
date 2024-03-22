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

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;
	
	UFUNCTION()
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;	

	UPROPERTY()
	TArray<class AActor*> outActors;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayerStart> playerStartFactory;



};



