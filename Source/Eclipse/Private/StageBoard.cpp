// Fill out your copyright notice in the Description page of Project Settings.


#include "StageBoard.h"

// Sets default values
AStageBoard::AStageBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boardMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("checkerMesh"));
	SetRootComponent(boardMesh);
	boardMesh->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void AStageBoard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStageBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

