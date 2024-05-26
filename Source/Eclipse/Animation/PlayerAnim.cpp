// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void UPlayerAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	me = Cast<APlayerCharacter>(TryGetPawnOwner());
	me->WeaponChangeDele.AddUObject(this, &UPlayerAnim::UpdateWeaponEquipState);
}

void UPlayerAnim::AnimNotify_ReloadStart() const
{
	GetWorld()->GetTimerManager().ClearTimer(me->ShootEnableHandle);
	me->CanShoot = false;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		me->CanShoot = true;
	}), 2.3f, false);
}

void UPlayerAnim::AnimNotify_ReloadEnd() const
{
	if (me->weaponArray[0] == true)
	{
		if (me->maxRifleAmmo < (40 - me->curRifleAmmo))
		{
			me->curRifleAmmo += me->maxRifleAmmo;
			me->maxRifleAmmo = 0;
		}
		else
		{
			me->maxRifleAmmo -= (40 - me->curRifleAmmo);
			me->curRifleAmmo += (40- me->curRifleAmmo);
		}
	}
	else if (me->weaponArray[1] == true)
	{
		if (me->maxSniperAmmo < (5  - me->curSniperAmmo))
		{
			me->curSniperAmmo += me->maxSniperAmmo;
			me->maxSniperAmmo = 0;
		}
		else
		{
			me->maxSniperAmmo -= (5 - me->curSniperAmmo);
			me->curSniperAmmo += (5- me->curSniperAmmo);
		}
	}
	else if (me->weaponArray[2] == true)
	{
		if (me->maxPistolAmmo < (8  - me->curPistolAmmo))
		{
			me->curPistolAmmo += me->maxPistolAmmo;
			me->maxPistolAmmo = 0;
		}
		else
		{
			me->maxPistolAmmo -= (8 - me->curPistolAmmo);
			me->curPistolAmmo += (8 - me->curPistolAmmo);
		}
	}
	else if (me->weaponArray[3] == true)
	{
		if (me->maxM249Ammo < (100  - me->curM249Ammo))
		{
			me->curM249Ammo += me->maxPistolAmmo;
			me->maxM249Ammo = 0;
		}
		else
		{
			me->maxM249Ammo -= (100 - me->curM249Ammo);
			me->curM249Ammo += (100  - me->curM249Ammo);
		}
	}
}


void UPlayerAnim::UpdateWeaponEquipState()
{
	if(me->weaponArray[2]==true)
	{
		bPistol=true;
	}
	else
	{
		bPistol=false;
	}
}

