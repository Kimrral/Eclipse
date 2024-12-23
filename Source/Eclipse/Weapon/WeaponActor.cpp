// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled=false;

	weaponMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("weaponMesh"));
	SetRootComponent(weaponMesh);

	weaponMesh->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
	weaponMesh->SetRenderCustomDepth(false);
	
}

// Called every frame
void AWeaponActor::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

}


