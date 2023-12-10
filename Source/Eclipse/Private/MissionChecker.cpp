// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionChecker.h"

// Sets default values
AMissionChecker::AMissionChecker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	checkerMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("checkerMesh"));
	SetRootComponent(checkerMesh);
	checkerMesh->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void AMissionChecker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMissionChecker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

