// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RewardManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECLIPSE_API URewardManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URewardManagerComponent();

	UFUNCTION(Server, Reliable)
	void DropRewardServer(const FTransform& EnemyTransform);

	UFUNCTION(Server, Reliable)
	void DropSniperServer(const FTransform& EnemyTransform) const;

	UFUNCTION(Server, Reliable)
	void DropM249Server(const FTransform& EnemyTransform) const;

	UFUNCTION()
	void DropAmmunition(const FTransform& EnemyTransform) const;

	UFUNCTION()
	void DropFirstItem(const FTransform& EnemyTransform) const;
	
	UFUNCTION()
	void DropSecondItem(const FTransform& EnemyTransform) const;

	
	
	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class ASniperActor> SniperWeaponFactory;

	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class AM249Actor> M249WeaponFactory;

	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class APickableActor> PickableActorListFirst;

	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class APickableActor> PickableActorListSecond;

	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class APickableActor> PickableActorListThird;

	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class APickableActor> PickableActorListFourth;

	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class APickableActor> PickableActorListFifth;

	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class APickableActor> PickableActorListSixth;

	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class ARifleAmmoPack> RifleAmmoPack;

	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class ASniperAmmoPack> SniperAmmoPack;	
	
	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class APistolAmmoPack> PistolAmmoPack;

	UPROPERTY(EditAnywhere, Category=Factory)
	TSubclassOf<class AM249AmmoPack> M249AmmoPack;	
};
