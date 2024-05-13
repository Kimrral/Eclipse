// Copyright Epic Games, Inc. All Rights Reserved.

#include "EclipseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "EclipseGameState.h"
#include "Eclipse/Character/PlayerCharacter.h"

AEclipseGameMode::AEclipseGameMode()
{
	GameStateClass = AEclipseGameState::StaticClass();
}

// 첫 실행 시 스폰될 플레이어 스타트 지점 오버라이딩
AActor* AEclipseGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	//기존의 플레이어 스타트 액터 리턴값 변경
	TArray<class AActor*> OutActors;
	TArray<class APlayerStart*> TargetPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), playerStartFactory, OutActors);
	for (const auto PlayerStarts : OutActors)
	{
		if (const auto PlayerStart = Cast<APlayerStart>(PlayerStarts))
		{
			if (PlayerStart && PlayerStart->PlayerStartTag == FName("Hideout"))
			{
				const FVector Center = PlayerStart->GetActorLocation();
				FCollisionQueryParams Params;
				bool AlreadyLocated = false;
				if (TArray<FOverlapResult> HitObj; GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(50), Params))
				{
					for (int i = 0; i < HitObj.Num(); ++i)
					{
						if (Cast<APlayerCharacter>(HitObj[i].GetActor()))
						{
							AlreadyLocated = true;
							break;
						}						
					}
				}
				if(AlreadyLocated)
				{
					continue;
				}
				TargetPlayerStarts.Add(PlayerStart);
			}
		}
	}
	if (const auto PlayerStartRandIndex = FMath::RandRange(0, TargetPlayerStarts.Num() - 1); TargetPlayerStarts.IsValidIndex(PlayerStartRandIndex))
	{
		APlayerStart* const TargetPlayerStart = TargetPlayerStarts[PlayerStartRandIndex];
		return TargetPlayerStart;
	}
	return nullptr;
}
