// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Item/PickableActor.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/Game/EclipsePlayerState.h"

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

void APickableActor::SetSimulateDisabled()
{
	RootMesh->SetSimulatePhysics(false);
}

void APickableActor::AddToInventory(APlayerCharacter* PlayerCharacter) const
{
	if(PlayerCharacter)
	{
		if(AEclipsePlayerState* EclipsePlayerState = Cast<AEclipsePlayerState>(PlayerCharacter->GetPlayerState()))
		{
			EclipsePlayerState->AddToInventory(PlayerCharacter, InventoryItemStruct);
		}
	}
}

void APickableActor::BeginPlay()
{
	Super::BeginPlay();

	RootMesh->SetRenderCustomDepth(true);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &APickableActor::SetSimulateDisabled, 10.f, false);
}



