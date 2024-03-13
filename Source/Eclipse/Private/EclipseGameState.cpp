// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipseGameState.h"
#include "Eclipse/Eclipse.h"

void AEclipseGameState::HandleBeginPlay()
{
	EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("Begin"))
	Super::HandleBeginPlay();
	EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("End"))
}

void AEclipseGameState::OnRep_ReplicatedHasBegunPlay()
{
	EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("Begin"))
	Super::OnRep_ReplicatedHasBegunPlay();
	EC_LOG(LogECNetwork, Log, TEXT("%s"), TEXT("End"))
}


