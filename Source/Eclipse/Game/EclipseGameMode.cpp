// Copyright Epic Games, Inc. All Rights Reserved.

#include "EclipseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Eclipse/Eclipse.h"
#include "EclipseGameState.h"

AEclipseGameMode::AEclipseGameMode()
{		
	GameStateClass = AEclipseGameState::StaticClass();
}

// 첫 실행 시 스폰될 플레이어 스타트 지점 오버라이딩
AActor* AEclipseGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// 기존의 플레이어 스타트 액터 리턴값 변경
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), playerStartFactory, outActors);
	// outActors 배열의 0번째 액터 리턴
	return outActors[0];
}

void AEclipseGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("Begin"))
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("End"))
}

APlayerController* AEclipseGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("Begin"))
	APlayerController* NewPlayerController= Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("End"))
	return NewPlayerController;
}

void AEclipseGameMode::PostLogin(APlayerController* NewPlayer)
{
	EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("Begin"))
	Super::PostLogin(NewPlayer);
	UNetDriver* NetDriver = GetNetDriver();
	if(NetDriver)
	{
		if(NetDriver->ClientConnections.Num()==0)
		{
			EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("No Client Connections"));
		}
		else
		{
			for(const auto& Connection : NetDriver->ClientConnections)
			{
				EC_LOG(LogECNetwork, Log, TEXT("Client Connections : %s"), *Connection.GetName());
			}
		}
	}
	else
	{
		EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("No Net Driver"));
	}
	
	EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("End"))
}

void AEclipseGameMode::StartPlay()
{
	EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("Begin"))
	Super::StartPlay();
	EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("End"))
}

