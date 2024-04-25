// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadPlayerContainer.h"

// Sets default values
ADeadPlayerContainer::ADeadPlayerContainer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	DeadBodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DeadBodyMesh"));
	SetRootComponent(DeadBodyMesh);
	DeadBodyMesh->SetGenerateOverlapEvents(true);

}




