// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardContainer.h"

#include "HackingConsole.h"
#include "M249AmmoActor.h"
#include "PistolAmmoActor.h"
#include "PlayerCharacter.h"
#include "RifleAmmoActor.h"
#include "SniperAmmoActor.h"
#include "Components/SphereComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARewardContainer::ARewardContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	containerMesh=CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("containerMesh"));
	SetRootComponent(containerMesh);

	playerDetectCollision=CreateDefaultSubobject<USphereComponent>(TEXT("playerDetectCollision"));
	playerDetectCollision->SetupAttachment(RootComponent);

	playerDetectCollision->OnComponentBeginOverlap.AddDynamic(this, &ARewardContainer::SphereOnOverlap);
	playerDetectCollision->OnComponentEndOverlap.AddDynamic(this, &ARewardContainer::SphereEndOverlap);

	curBoxHP=maxBoxHP;

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

void ARewardContainer::SphereOnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		auto player=Cast<APlayerCharacter>(OtherActor);
		if(player)
		{
			containerMesh->SetRenderCustomDepth(true);
		}
	}
}

void ARewardContainer::SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		auto player=Cast<APlayerCharacter>(OtherActor);
		if(player)
		{
			//containerMesh->SetRenderCustomDepth(false);
		}
	}
}

void ARewardContainer::DropReward()
{
	DropAmmo();
	DropConsole();
}

void ARewardContainer::DropAmmo()
{
	auto randIndex = FMath::RandRange(0, 3);
	if(randIndex==0)
	{
		auto rifleAmmo = GetWorld()->SpawnActor<ARifleAmmoActor>(rifleAmmoFactory, GetActorLocation(), GetActorRotation());
		if(rifleAmmo)
		{
			FVector force = FVector(0, 0, 2500);
			FVector loc = rifleAmmo->GetActorUpVector();
			rifleAmmo->ammoMesh->AddImpulseAtLocation(force, loc);
		}
	}
	else if(randIndex==1)
	{
		auto sniperAmmo = GetWorld()->SpawnActor<ASniperAmmoActor>(sniperAmmoFactory, GetActorLocation(), GetActorRotation());
		if(sniperAmmo)
		{
			FVector force = FVector(0, 0, 2500);
			FVector loc = sniperAmmo->GetActorUpVector();
			sniperAmmo->ammoMesh->AddImpulseAtLocation(force, loc);
		}
	}
	else if(randIndex==2)
	{
		auto pistolAmmo = GetWorld()->SpawnActor<APistolAmmoActor>(pistolAmmoFactory, GetActorLocation(), GetActorRotation());
		if(pistolAmmo)
		{
			FVector force = FVector(0, 0, 2500);
			FVector loc = pistolAmmo->GetActorUpVector();
			pistolAmmo->ammoMesh->AddImpulseAtLocation(force, loc);
		}
	}
	else if(randIndex==3)
	{
		auto m249Ammo = GetWorld()->SpawnActor<AM249AmmoActor>(M249AmmoFactory, GetActorLocation(), GetActorRotation());
		if(m249Ammo)
		{
			FVector force = FVector(0, 0, 2500);
			FVector loc = m249Ammo->GetActorUpVector();
			m249Ammo->ammoMesh->AddImpulseAtLocation(force, loc);
		}
	}
}

void ARewardContainer::DropConsole()
{
	auto HackingConsole = GetWorld()->SpawnActor<AHackingConsole>(HackingConsoleFactory, GetActorLocation(), GetActorRotation());
	if(HackingConsole)
	{
		FVector force = FVector(0, 0, 2500);
		FVector loc = HackingConsole->GetActorUpVector();
		HackingConsole->rewardMesh->AddImpulseAtLocation(force, loc);
	}
}

void ARewardContainer::BoxDestroyed()
{
	DropReward();
	playerDetectCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	containerMesh->SetRenderCustomDepth(false);
	bDestroyed=true;
}
