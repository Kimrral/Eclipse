// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Item/PickableActor.h"

// Sets default values
APickableActor::APickableActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled=false;

	RootMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));
	SetRootComponent(RootMesh);
	RootMesh->SetCollisionObjectType(ECC_WorldDynamic);
	RootMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	RootMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	RootMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	RootMesh->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);
	RootMesh->SetSimulatePhysics(true);

	bReplicates=true;
}

// Called when the game starts or when spawned
void APickableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

