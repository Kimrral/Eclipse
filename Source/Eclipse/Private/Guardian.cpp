// Fill out your copyright notice in the Description page of Project Settings.


#include "Guardian.h"

void AGuardian::DropReward()
{
	Super::DropReward();

	DropAmmo();
	DropAmmo();
	auto randNum = FMath::RandRange(0, 3);
	if(randNum==0)
	{
		DropMagazine();
	}
}
