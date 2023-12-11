// Fill out your copyright notice in the Description page of Project Settings.


#include "MagActor.h"

// Sets default values
AMagActor::AMagActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	magMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("magMesh"));
	SetRootComponent(magMesh);
	magMesh->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AMagActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMagActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

