// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardContainer.h"

#include "Eclipse/Item/AdrenalineSyringe.h"
#include "Eclipse/Item/FirstAidKitActor.h"
#include "Eclipse/Item/HackingConsole.h"
#include "Eclipse/Item/M249MagActor.h"
#include "Eclipse/Item/MedKitActor.h"
#include "Eclipse/Item/MilitaryDevice.h"
#include "Eclipse/Item/MilitaryLaptop.h"
#include "Eclipse/Item/PistolMagActor.h"
#include "Eclipse/Item/RewardManagerComponent.h"
#include "Eclipse/Item/RifleMagActor.h"
#include "Eclipse/Item/SniperMagActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARewardContainer::ARewardContainer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Root Mesh
	ContainerMesh = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("containerMesh"));
	SetRootComponent(ContainerMesh);
	// Reward Manager
	RewardManager = CreateDefaultSubobject<URewardManagerComponent>(TEXT("RewardManagerComponent"));

	CurBoxHP = MaxBoxHP;
	bReplicates = true;
}

void ARewardContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARewardContainer, IsBoxDestroyed);
}

// Called when the game starts or when spawned
void ARewardContainer::BeginPlay()
{
	Super::BeginPlay();

	ContainerMesh->SetRenderCustomDepth(false);
	ContainerMesh->SetSimulatePhysics(true);
}

void ARewardContainer::BoxDestroyed()
{
	IsBoxDestroyed = true;
	OnRep_IsBoxDestroyed();
	RewardManager->DropRewardServer(GetActorTransform());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ContainerBreakSound, this->GetActorLocation());
}


void ARewardContainer::OnRep_IsBoxDestroyed()
{
	SetLifeSpan(5.f);
	containerDele.ExecuteIfBound(GetActorLocation());
}
