// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardContainer.h"

#include "Eclipse/Item/HackingConsole.h"
#include "Eclipse/Item/M249MagActor.h"
#include "Eclipse/Item/PistolMagActor.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/Item/RifleMagActor.h"
#include "Eclipse/Item/SniperMagActor.h"
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

	//DropForce=FVector(80);
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
	DropConsole();
}



void ARewardContainer::DropConsole()
{
	auto HackingConsole = GetWorld()->SpawnActor<AHackingConsole>(HackingConsoleFactory, GetActorLocation(), GetActorRotation());
	if(HackingConsole)
	{
		//FVector loc = HackingConsole->GetActorUpVector();
		//HackingConsole->rewardMesh->AddImpulseAtLocation(DropForce, loc);
	}
}

void ARewardContainer::DropMagazine()
{

	auto randIndex = FMath::RandRange(0, 3);
	if(randIndex==0)
	{
		auto RifleMagActor = GetWorld()->SpawnActor<ARifleMagActor>(RifleMagActorFactory, GetActorLocation(), GetActorRotation());
		if(RifleMagActor)
		{
			//FVector loc = RifleMagActor->GetActorUpVector();
			//RifleMagActor->magMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
	else if(randIndex==1)
	{
		auto SniperMagActor = GetWorld()->SpawnActor<ASniperMagActor>(SniperMagActorFactory, GetActorLocation(), GetActorRotation());
		if(SniperMagActor)
		{
			//FVector loc = SniperMagActor->GetActorUpVector();
			//SniperMagActor->magMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
	else if(randIndex==2)
	{
		auto PistolMagActor = GetWorld()->SpawnActor<APistolMagActor>(PistolMagActorFactory, GetActorLocation(), GetActorRotation());
		if(PistolMagActor)
		{
			//FVector loc = PistolMagActor->GetActorUpVector();
			//PistolMagActor->magMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
	else if(randIndex==3)
	{
		auto M249MagActor = GetWorld()->SpawnActor<AM249MagActor>(M249MagActorFactory, GetActorLocation(), GetActorRotation());
		if(M249MagActor)
		{
			//FVector loc = M249MagActor->GetActorUpVector();
			//M249MagActor->magMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
}

void ARewardContainer::BoxDestroyed()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), containerBreakSound, this->GetActorLocation());
	DropReward();
	playerDetectCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	containerMesh->SetRenderCustomDepth(false);
	bDestroyed=true;
}
