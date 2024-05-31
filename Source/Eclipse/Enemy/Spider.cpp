// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Spider.h"

#include "Eclipse/AI/EnemyFSM.h"
#include "Eclipse/Item/PoisonOfSpider.h"


ASpider::ASpider()
{
	// Enemy FSM
	EnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("EnemyFSM"));
}

void ASpider::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetRelativeScale3D(FVector(0.3f));
	PlayAnimMontage(SpiderSpawnMontage);
}
