// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardContainer.h"

#include "Eclipse/Item/HackingConsole.h"
#include "Eclipse/Item/M249MagActor.h"
#include "Eclipse/Item/PistolMagActor.h"
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

	containerMesh=CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("containerMesh"));
	SetRootComponent(containerMesh);
	
	curBoxHP=maxBoxHP;

	SetReplicates(true);

}

void ARewardContainer::OnRep_IsBoxDestroyed()
{
	containerDele.ExecuteIfBound(GetActorLocation());
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

	containerMesh->SetRenderCustomDepth(false);
	containerMesh->SetSimulatePhysics(true);
}

// Called every frame
void ARewardContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARewardContainer::DropReward()
{
	DropConsole();
	DropMagazine();
}

void ARewardContainer::DropConsole()
{
	GetWorld()->SpawnActor<AHackingConsole>(HackingConsoleFactory, GetActorLocation(), GetActorRotation());	
}

void ARewardContainer::DropMagazine()
{

	auto randIndex = FMath::RandRange(0, 3);
	if(randIndex==0)
	{
		GetWorld()->SpawnActor<ARifleMagActor>(RifleMagActorFactory, GetActorLocation(), GetActorRotation());		
	}
	else if(randIndex==1)
	{
		GetWorld()->SpawnActor<ASniperMagActor>(SniperMagActorFactory, GetActorLocation(), GetActorRotation());	
	}
	else if(randIndex==2)
	{
		GetWorld()->SpawnActor<APistolMagActor>(PistolMagActorFactory, GetActorLocation(), GetActorRotation());	
	}
	else if(randIndex==3)
	{
		GetWorld()->SpawnActor<AM249MagActor>(M249MagActorFactory, GetActorLocation(), GetActorRotation());
	}
}

void ARewardContainer::BoxDestroyed()
{
	IsBoxDestroyed=true;		
	containerDele.ExecuteIfBound(GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), containerBreakSound, this->GetActorLocation());
	DropReward();		
	
}
