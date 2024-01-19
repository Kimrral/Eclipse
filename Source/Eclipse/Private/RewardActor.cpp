// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardActor.h"

#include "Components/SphereComponent.h"

// Sets default values
ARewardActor::ARewardActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled=false;

	rewardMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("rewardMesh"));
	SetRootComponent(rewardMesh);
	rewardMesh->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ARewardActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARewardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

