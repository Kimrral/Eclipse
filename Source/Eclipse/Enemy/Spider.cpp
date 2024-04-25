// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Spider.h"

#include "Eclipse/Item/PoisonOfSpider.h"


void ASpider::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetRelativeScale3D(FVector(0.3f));
	PlayAnimMontage(SpiderSpawnMontage);
}

void ASpider::DropReward()
{
	FActorSpawnParameters Param;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<APoisonOfSpider>(PoisonOfSpiderFactory, GetActorLocation(), GetActorRotation(), Param);	
}
