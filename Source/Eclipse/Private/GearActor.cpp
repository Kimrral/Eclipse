// Fill out your copyright notice in the Description page of Project Settings.


#include "GearActor.h"

// Sets default values
AGearActor::AGearActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled=false;

	gearMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("gearMesh"));
	SetRootComponent(gearMesh);
	gearMesh->SetCollisionObjectType(ECC_WorldDynamic);
	gearMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	gearMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	gearMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	gearMesh->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);
	gearMesh->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void AGearActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGearActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

