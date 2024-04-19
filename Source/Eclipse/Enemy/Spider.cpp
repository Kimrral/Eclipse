// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Spider.h"

void ASpider::BeginPlay()
{
	Super::BeginPlay();

	PlayAnimMontage(SpiderSpawnMontage);
}
