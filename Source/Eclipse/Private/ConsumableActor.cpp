// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsumableActor.h"

// Sets default values
AConsumableActor::AConsumableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	consumeMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("gearMesh"));
	SetRootComponent(consumeMesh);
	consumeMesh->SetCollisionObjectType(ECC_WorldDynamic);
	consumeMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	consumeMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	consumeMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	consumeMesh->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);
	consumeMesh->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void AConsumableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AConsumableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

