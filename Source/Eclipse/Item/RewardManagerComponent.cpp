// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Item/RewardManagerComponent.h"

#include "M249AmmoPack.h"
#include "PickableActor.h"
#include "PistolAmmoPack.h"
#include "RifleAmmoPack.h"
#include "SniperAmmoPack.h"
#include "Eclipse/Weapon/M249Actor.h"
#include "Eclipse/Weapon/SniperActor.h"

// 이 컴포넌트의 기본 값을 설정합니다.
URewardManagerComponent::URewardManagerComponent()
{
	// 이 컴포넌트가 게임 시작 시 초기화되고 매 프레임마다 Tick()이 호출되도록 설정합니다.
	// 필요하지 않은 경우 성능 향상을 위해 이러한 기능을 끌 수 있습니다.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

// 서버에서 저격총을 드랍하는 함수의 구현부입니다.
void URewardManagerComponent::DropSniperServer_Implementation(const FTransform& EnemyTransform) const
{
	// 스폰 파라미터 설정
	FActorSpawnParameters Param;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// 저격총 액터를 스폰합니다.
	GetWorld()->SpawnActor<ASniperActor>(SniperWeaponFactory, EnemyTransform, Param);
}

// 서버에서 M249를 드랍하는 함수의 구현부입니다.
void URewardManagerComponent::DropM249Server_Implementation(const FTransform& EnemyTransform) const
{
	// 스폰 파라미터 설정
	FActorSpawnParameters Param;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// M249 액터를 스폰합니다.
	GetWorld()->SpawnActor<AM249Actor>(M249WeaponFactory, EnemyTransform, Param);
}

// 탄약을 드랍하는 함수입니다.
void URewardManagerComponent::DropAmmunition(const FTransform& EnemyTransform) const
{
	// 스폰 파라미터 설정
	FActorSpawnParameters Param;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// 랜덤 인덱스를 생성하여 탄약 팩을 스폰합니다.
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

// 첫 번째 아이템을 드랍하는 함수입니다.
void URewardManagerComponent::DropFirstItem(const FTransform& EnemyTransform) const
{
	// 스폰 파라미터 설정
	FActorSpawnParameters Param;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// 랜덤 인덱스를 생성하여 첫 번째 아이템 리스트에서 아이템을 스폰합니다.
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

// 두 번째 아이템을 드랍하는 함수입니다.
void URewardManagerComponent::DropSecondItem(const FTransform& EnemyTransform) const
{
	// 스폰 파라미터 설정
	FActorSpawnParameters Param;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// 랜덤 인덱스를 생성하여 두 번째 아이템 리스트에서 아이템을 스폰합니다.
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

// 서버에서 보상을 드랍하는 함수의 구현부입니다.
void URewardManagerComponent::DropRewardServer_Implementation(const FTransform& EnemyTransform)
{
	// 첫 번째 아이템, 두 번째 아이템, 그리고 탄약을 드랍합니다.
	DropFirstItem(EnemyTransform);
	DropSecondItem(EnemyTransform);
	DropAmmunition(EnemyTransform);
}
