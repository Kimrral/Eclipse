// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch.h"


void ACrunch::DropReward()
{
	Super::DropReward();

	DropAmmo();
	DropAmmo();
	DropAmmo();
	DropAmmo();	
	DropMagazine();
	DropMagazine();
	DropGear();
	DropGear();

}
