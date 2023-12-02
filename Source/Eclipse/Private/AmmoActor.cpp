// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoActor.h"

#include "PlayerCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
AAmmoActor::AAmmoActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ammoMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("weaponMesh"));
	SetRootComponent(ammoMesh);
	ammoMesh->SetGenerateOverlapEvents(true);

	playerTraceSphereCollision=CreateDefaultSubobject<USphereComponent>(TEXT("playerTraceSphereCollision"));
	playerTraceSphereCollision->SetupAttachment(RootComponent);
	playerTraceSphereCollision->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AAmmoActor::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AAmmoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
