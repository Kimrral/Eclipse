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
		if(me->maxRifleAmmo<(40+me->SetRifleAdditionalMagazine())-me->curRifleAmmo)
		{
			me->curRifleAmmo+=me->maxRifleAmmo;
			me->maxRifleAmmo=0;
		}
		else
		{
			me->maxRifleAmmo-=((40+me->SetRifleAdditionalMagazine())-me->curRifleAmmo);
			me->curRifleAmmo+=((40+me->SetRifleAdditionalMagazine())-me->curRifleAmmo);
		}
		UE_LOG(LogTemp, Warning, TEXT("MaxRifleAmmo:%d"), me->maxRifleAmmo)
	}
	else if(me->weaponArray[1]==true)
	{
		if(me->maxSniperAmmo<(5+me->SetSniperAdditionalMagazine())-me->curSniperAmmo)
		{
			me->curSniperAmmo+=me->maxSniperAmmo;
			me->maxSniperAmmo=0;
		}
		else
		{
			me->maxSniperAmmo-=((5+me->SetSniperAdditionalMagazine())-me->curSniperAmmo);
			me->curSniperAmmo+=((5+me->SetSniperAdditionalMagazine())-me->curSniperAmmo);
		}
		UE_LOG(LogTemp, Warning, TEXT("MaxSniperAmmo:%d"), me->maxSniperAmmo)
	}
	else if(me->weaponArray[2]==true)
	{
		if(me->maxPistolAmmo<(8+me->SetPistolAdditionalMagazine())-me->curPistolAmmo)
		{
			me->curPistolAmmo+=me->maxPistolAmmo;
			me->maxPistolAmmo=0;
		}
		else
		{
			me->maxPistolAmmo-=((8+me->SetPistolAdditionalMagazine())-me->curPistolAmmo);
			me->curPistolAmmo+=((8+me->SetPistolAdditionalMagazine())-me->curPistolAmmo);
		}
		UE_LOG(LogTemp, Warning, TEXT("MaxPistolAmmo:%d"), me->maxPistolAmmo)
	}
	else if(me->weaponArray[3]==true)
	{
		if(me->maxM249Ammo<(100+me->SetM249AdditionalMagazine())-me->curM249Ammo)
		{
			me->curM249Ammo+=me->maxPistolAmmo;
			me->maxM249Ammo=0;
		}
		else
		{
			me->maxM249Ammo-=((100+me->SetM249AdditionalMagazine())-me->curM249Ammo);
			me->curM249Ammo+=((100+me->SetM249AdditionalMagazine())-me->curM249Ammo);
		}
		UE_LOG(LogTemp, Warning, TEXT("MaxM249Ammo:%d"), me->maxM249Ammo)
	}
	me->CanShoot=true;
}

void UPlayerAnim::AnimNotify_LeftPlant()
{
	if(me->HasAuthority())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), walkSound);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), walkSound, me->GetActorLocation());
	}
}

void UPlayerAnim::AnimNotify_RightPlant()
{
	if(me->HasAuthority())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), walkSound);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), walkSound, me->GetActorLocation());
	}
}
