// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Prop/Trader.h"

// Sets default values
ATrader::ATrader()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled=false;

	TraderCharacterMesh=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TraderCharacterMesh"));
	SetRootComponent(TraderCharacterMesh);
	TraderCharacterMesh->SetGenerateOverlapEvents(true);

}
