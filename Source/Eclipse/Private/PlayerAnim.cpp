// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"

#include "PlayerCharacter.h"

void UPlayerAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	me =  Cast<APlayerCharacter>(TryGetPawnOwner());

}

void UPlayerAnim::AnimNotify_ReloadStart()
{
	GetWorld()->GetTimerManager().ClearTimer(me->shootEnableHandle);
	me->CanShoot=false;
}

void UPlayerAnim::AnimNotify_ReloadEnd()
{
	me->curRifleAmmo=30;
	me->CanShoot=true;

}
