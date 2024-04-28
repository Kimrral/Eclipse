// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadPlayerContainer.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ADeadPlayerContainer::ADeadPlayerContainer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	DeadBodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DeadBodyMesh"));
	SetRootComponent(DeadBodyMesh);
	DeadBodyMesh->SetGenerateOverlapEvents(true);

	bReplicates=true;

}

void ADeadPlayerContainer::BeginPlay()
{
	Super::BeginPlay();
}
	

void ADeadPlayerContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADeadPlayerContainer, DeadPlayerInventoryStructArray);
	DOREPLIFETIME(ADeadPlayerContainer, DeadPlayerInventoryStackArray);
	DOREPLIFETIME(ADeadPlayerContainer, DeadPlayerGearSlotArray);
}




