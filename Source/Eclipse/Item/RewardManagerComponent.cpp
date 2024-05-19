// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Item/RewardManagerComponent.h"

#include "M249AmmoPack.h"
#include "PickableActor.h"
#include "PistolAmmoPack.h"
#include "RifleAmmoPack.h"
#include "SniperAmmoPack.h"
#include "Eclipse/Weapon/SniperActor.h"

// Sets default values for this component's properties
URewardManagerComponent::URewardManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void URewardManagerComponent::DropSniperServer_Implementation(const FTransform& EnemyTransform) const
{
	FActorSpawnParameters Param;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<ASniperActor>(SniperWeaponFactory, EnemyTransform, Param);
}

void URewardManagerComponent::DropAmmunition(const FTransform& EnemyTransform) const
{
	FActorSpawnParameters Param;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (const auto RandIndex = FMath::RandRange(0, 3); RandIndex == 0)
	{
		GetWorld()->SpawnActor<ARifleAmmoPack>(RifleAmmoPack, EnemyTransform, Param);
	}
	else if (RandIndex == 1)
	{
		GetWorld()->SpawnActor<ASniperAmmoPack>(SniperAmmoPack, EnemyTransform, Param);
	}
	else if (RandIndex == 2)
	{
		GetWorld()->SpawnActor<APistolAmmoPack>(PistolAmmoPack, EnemyTransform, Param);
	}
	else if (RandIndex == 3)
	{
		GetWorld()->SpawnActor<AM249AmmoPack>(M249AmmoPack, EnemyTransform, Param);
	}
}

void URewardManagerComponent::DropFirstItem(const FTransform& EnemyTransform) const
{
	FActorSpawnParameters Param;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (const auto RandIndex = FMath::RandRange(0, 2); RandIndex == 0)
	{
		GetWorld()->SpawnActor<APickableActor>(PickableActorListFirst, EnemyTransform, Param);
	}
	else if (RandIndex == 1)
	{
		GetWorld()->SpawnActor<APickableActor>(PickableActorListSecond, EnemyTransform, Param);
	}
	else if (RandIndex == 2)
	{
		GetWorld()->SpawnActor<APickableActor>(PickableActorListThird, EnemyTransform, Param);
	}
}

void URewardManagerComponent::DropSecondItem(const FTransform& EnemyTransform) const
{
	FActorSpawnParameters Param;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (const auto RandIndex = FMath::RandRange(0, 2); RandIndex == 0)
	{
		GetWorld()->SpawnActor<APickableActor>(PickableActorListFourth, EnemyTransform, Param);
	}
	else if (RandIndex == 1)
	{
		GetWorld()->SpawnActor<APickableActor>(PickableActorListFifth, EnemyTransform, Param);
	}
	else if (RandIndex == 2)
	{
		GetWorld()->SpawnActor<APickableActor>(PickableActorListSixth, EnemyTransform, Param);
	}
}

void URewardManagerComponent::DropRewardServer_Implementation(const FTransform& EnemyTransform)
{
	DropFirstItem(EnemyTransform);
	DropSecondItem(EnemyTransform);
	DropAmmunition(EnemyTransform);
}
