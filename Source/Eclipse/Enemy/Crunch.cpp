// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch.h"

#include "Eclipse/Item/RewardManagerComponent.h"


void ACrunch::SetDissolveValue(float Value)
{
	return;
}

void ACrunch::OnDestroy()
{
	if (HasAuthority())
	{
		RewardManager->DropRewardServer(GetActorTransform());
		SetLifeSpan(30.f);
	}
}

void ACrunch::SetDissolveMaterial()
{
	return;
}
