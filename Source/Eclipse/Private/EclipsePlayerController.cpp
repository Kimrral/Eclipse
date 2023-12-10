// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipsePlayerController.h"

#include "Eclipse/EclipseGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AEclipsePlayerController::Respawn(APlayerCharacter* me)
{
	if(me)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), playerStartFactory, outActors);
		int ClosestIndex = 0;
		for(int i=0; i<outActors.Num(); i++)
		{
			// [ClosestIndex]번째의 액터와 사망지점과의 거리
			float ClosestDist = FVector::Dist(outActors[ClosestIndex]->GetActorLocation(), me->DeathPosition);
			// [i]번째의 액터와 사망지점과의 거리
			float NextDist = FVector::Dist(outActors[i]->GetActorLocation(),  me->DeathPosition);

			// 만약 이번 대상이 현재 대상보다 가깝다면,
			if (NextDist < ClosestDist)
			{
				// 가장 가까운 대상으로 변경하기
				ClosestIndex = i;
			}	
		}
		auto playerStart = Cast<APlayerStart>(outActors[ClosestIndex]);
		if(playerStart)
		{				
			auto GM=Cast<AEclipseGameMode>(GetWorld()->GetAuthGameMode());
			if(GM)
			{			
				GM->RestartPlayerAtPlayerStart(this, playerStart);
			}
		}
	}
}

