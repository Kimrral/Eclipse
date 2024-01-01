// Fill out your copyright notice in the Description page of Project Settings.


#include "Stash.h"

// Sets default values
AStash::AStash()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	stashMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("stashMesh"));
	SetRootComponent(stashMesh);
	stashMesh->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AStash::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStash::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

