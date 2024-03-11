// Copyright Epic Games, Inc. All Rights Reserved.

#include "EclipseGameMode.h"
#include "EclipseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AEclipseGameMode::AEclipseGameMode()
{
	curRifleAmmo=40;
	curSniperAmmo=5;
	curPistolAmmo=8;
	curM249Ammo=100;
	maxRifleAmmo=80;
	maxSniperAmmo=10;
	maxPistolAmmo=16;
	maxM249Ammo=200;
	
}

// 첫 실행 시 스폰될 플레이어 스타트 지점 오버라이딩
AActor* AEclipseGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// 기존의 플레이어 스타트 액터 리턴값 변경
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), playerStartFactory, outActors);
	// outActors 배열의 0번째 액터 리턴
	return outActors[0];
}

