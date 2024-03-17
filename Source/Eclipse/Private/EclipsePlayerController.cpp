// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipsePlayerController.h"

#include "Eclipse/EclipseGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"


//사망지점에서 가장 먼 플레이어 스타트 지점에서 리스폰
void AEclipsePlayerController::Respawn(APlayerCharacter* me)
{
	if(me)
	{
		// 레벨에 배치된 모든 플레이어 스타트 액터
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), playerStartFactory, outActors);
		int MaxDistIndex = 0;
		for(int i=0; i<outActors.Num(); i++)
		{
			// [MaxDistIndex]번째의 액터와 사망지점과의 거리
			float MaxDist = FVector::Dist(outActors[MaxDistIndex]->GetActorLocation(), me->DeathPosition);
			// [i]번째의 액터와 사망지점과의 거리
			float NextDist = FVector::Dist(outActors[i]->GetActorLocation(),  me->DeathPosition);

			// 만약 이번 대상이 현재 대상보다 멀다면,
			if (NextDist > MaxDist)
			{
				// 가장 거리가 먼 대상으로 변경하기
				MaxDistIndex = i;
			}	
		}
		// 플레이어 스타트 액터 캐스팅
		auto playerStart = Cast<APlayerStart>(outActors[MaxDistIndex]);
		if(playerStart)
		{				
			auto GM=Cast<AEclipseGameMode>(GetWorld()->GetAuthGameMode());
			if(GM)
			{
				// 게임모드의 리스타트 함수 호출
				GM->RestartPlayerAtPlayerStart(this, playerStart);
			}
		}
	}
}

