// Fill out your copyright notice in the Description page of Project Settings.


#include "QuitGameActor.h"

// Sets default values
AQuitGameActor::AQuitGameActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	quitGameMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("quitGameMesh"));
	SetRootComponent(quitGameMesh);
	quitGameMesh->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void AQuitGameActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AQuitGameActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

