// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipsePlayerController.h"

#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Game/EclipseGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AEclipsePlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	if (PlayerCharacter)
	{
		PlayerCharacter->Stat->OnHpChanged.AddUObject(this, &AEclipsePlayerController::UpdateTabWidget);
	}
}

void AEclipsePlayerController::UpdateTabWidget() const
{
	if (PlayerCharacter)
	{
		PlayerCharacter->UpdateTabWidgetHP();
	}
}


// void AEclipsePlayerController::Respawn(const APlayerCharacter* Me)
// {
// 	if (Me)
// 	{		
// 		TArray<class AActor*> OutActors;
// 		TArray<class APlayerStart*> TargetPlayerStarts;
// 		UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerStartFactory, OutActors);
// 		for (const auto PlayerStarts : OutActors)
// 		{
// 			if (const auto PlayerStart = Cast<APlayerStart>(PlayerStarts))
// 			{
// 				if (PlayerStart && PlayerStart->PlayerStartTag == FName("Hideout"))
// 				{
// 					if (const auto Gm = Cast<AEclipseGameMode>(GetWorld()->GetAuthGameMode()))
// 					{
// 						// 게임모드의 리스타트 함수 호출
// 						Gm->RestartPlayerAtPlayerStart(this,  PlayerStart);
// 						return;
// 					}
// 				}
// 			}
// 		}	
// 	}
// }




