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

	containerMesh = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("containerMesh"));
	SetRootComponent(containerMesh);

	curBoxHP = maxBoxHP;

	bReplicates = true;
}

void ARewardContainer::OnRep_IsBoxDestroyed() 
{
	SetLifeSpan(5.f);
	containerDele.ExecuteIfBound(GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), containerBreakSound, this->GetActorLocation());
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
void ARewardContainer::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARewardContainer::DropReward() const
{
	DropConsumable();
	DropETC();
	DropMagazine();
}

void ARewardContainer::DropConsole() const
{
	if (const AHackingConsole* HackingConsole = GetWorld()->SpawnActor<AHackingConsole>(HackingConsoleFactory, GetActorLocation(), GetActorRotation()))
	{
		HackingConsole->RootMesh->SetRenderCustomDepth(true);
	}
}

void ARewardContainer::DropMagazine() const
{
	if (const auto RandIndex = FMath::RandRange(0, 3); RandIndex == 0)
	{
		if (const ARifleMagActor* RifleMagActor = GetWorld()->SpawnActor<ARifleMagActor>(RifleMagActorFactory, GetActorLocation(), GetActorRotation()))
		{
			RifleMagActor->RootMesh->SetRenderCustomDepth(true);
		}
	}
	else if (RandIndex == 1)
	{
		if (const ASniperMagActor* SniperMagActor = GetWorld()->SpawnActor<ASniperMagActor>(SniperMagActorFactory, GetActorLocation(), GetActorRotation()))
		{
			SniperMagActor->RootMesh->SetRenderCustomDepth(true);
		}
	}
	else if (RandIndex == 2)
	{
		if (const APistolMagActor* PistolMagActor = GetWorld()->SpawnActor<APistolMagActor>(PistolMagActorFactory, GetActorLocation(), GetActorRotation()))
		{
			PistolMagActor->RootMesh->SetRenderCustomDepth(true);
		}
	}
	else if (RandIndex == 3)
	{
		if (const AM249MagActor* M249MagActor = GetWorld()->SpawnActor<AM249MagActor>(M249MagActorFactory, GetActorLocation(), GetActorRotation()))
		{
			M249MagActor->RootMesh->SetRenderCustomDepth(true);
		}
	}
}

void ARewardContainer::DropConsumable() const
{
	if (const auto RandIndex = FMath::RandRange(0, 2); RandIndex == 0)
	{
		if (const AAdrenalineSyringe* AdrenalineSyringe = GetWorld()->SpawnActor<AAdrenalineSyringe>(AdrenalineSyringeFactory, GetActorLocation(), GetActorRotation()))
		{
			AdrenalineSyringe->RootMesh->SetRenderCustomDepth(true);
		}
	}
	else if (RandIndex == 1)
	{
		if (const AFirstAidKitActor* FirstAidKit = GetWorld()->SpawnActor<AFirstAidKitActor>(FirstAidKitActorFactory, GetActorLocation(), GetActorRotation()))
		{
			FirstAidKit->RootMesh->SetRenderCustomDepth(true);
		}
	}
	else if (RandIndex == 2)
	{
		if (const AMedKitActor* MedKit = GetWorld()->SpawnActor<AMedKitActor>(MedKitActorFactory, GetActorLocation(), GetActorRotation()))
		{
			MedKit->RootMesh->SetRenderCustomDepth(true);
		}
	}
}

void ARewardContainer::DropETC() const
{
	if (const auto RandIndex = FMath::RandRange(0, 1); RandIndex == 0)
	{
		if (const AMilitaryDevice* MilitaryDevice = GetWorld()->SpawnActor<AMilitaryDevice>(MilitaryDeviceFactory, GetActorLocation(), GetActorRotation()))
		{
			MilitaryDevice->RootMesh->SetRenderCustomDepth(true);
		}
	}
	else if (RandIndex == 1)
	{
		if (const AMilitaryLaptop* MilitaryLaptop = GetWorld()->SpawnActor<AMilitaryLaptop>(MilitaryLaptopFactory, GetActorLocation(), GetActorRotation()))
		{
			MilitaryLaptop->RootMesh->SetRenderCustomDepth(true);
		}
	}
}

void ARewardContainer::BoxDestroyed()
{
	IsBoxDestroyed = true;
	OnRep_IsBoxDestroyed();
	DropReward();
}
