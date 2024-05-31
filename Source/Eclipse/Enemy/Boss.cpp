// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Boss.h"

#include "Eclipse/AI/EclipseBossAIController.h"

// Sets default values
ABoss::ABoss()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ABoss::SetAIController();
}


// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();
}


void ABoss::SetAIController()
{
	// AI Controller
	AIControllerClass = AEclipseBossAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


void ABoss::SetDissolveMaterial()
{
	return;
}

void ABoss::SetDissolveValue(float Value)
{
	return;
}
