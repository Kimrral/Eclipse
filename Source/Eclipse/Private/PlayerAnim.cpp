// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"

#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

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
	if(me->weaponArray[0]==true)
	{
		me->curRifleAmmo=30;
	}
	else if(me->weaponArray[1]==true)
	{
		me->curSniperAmmo=5;
	}
	else if(me->weaponArray[2]==true)
	{
		me->curPistolAmmo=8;
	}
	else if(me->weaponArray[3]==true)
	{
		me->curM249Ammo=100;
	}
	me->CanShoot=true;
}

void UPlayerAnim::AnimNotify_LeftPlant()
{
	UGameplayStatics::PlaySound2D(GetWorld(), walkSound);
}

void UPlayerAnim::AnimNotify_RightPlant()
{
	UGameplayStatics::PlaySound2D(GetWorld(), walkSound);
}
