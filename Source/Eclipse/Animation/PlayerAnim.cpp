// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"

#include "Eclipse/Character/PlayerCharacter.h"



void UPlayerAnim::NativeBeginPlay()
{
	// 부모 클래스의 NativeBeginPlay를 호출하여 기본적인 초기화를 수행합니다.
	Super::NativeBeginPlay();

	// 현재 애니메이션의 소유 액터를 APlayerCharacter 타입으로 캐스팅합니다.
	me = Cast<APlayerCharacter>(TryGetPawnOwner());

	// APlayerCharacter의 WeaponChangeDele 델리게이트에 UpdateWeaponEquipState 함수를 바인딩합니다.
	me->WeaponChangeDele.AddUObject(this, &UPlayerAnim::UpdateWeaponEquipState);
}

void UPlayerAnim::AnimNotify_ReloadStart() const
{
	// 재장전 애니메이션이 시작될 때 호출되는 함수입니다.

	// 총알 발사를 막기 위해 ShootEnableHandle 타이머를 지웁니다.
	GetWorld()->GetTimerManager().ClearTimer(me->ShootEnableHandle);

	// 총알 발사를 비활성화합니다.
	me->CanShoot = false;

	// 새로운 타이머를 설정하여 2.3초 후에 총알 발사를 다시 활성화합니다.
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		me->CanShoot = true;
	}), 2.3f, false);
}

// 재장전 함수
void UPlayerAnim::ReloadAmmo(FAmmoInfo &AmmoInfo)
{
	int32 AmmoNeeded = AmmoInfo.Capacity - AmmoInfo.CurAmmo;
	if (AmmoInfo.MaxAmmo < AmmoNeeded)
	{
		AmmoInfo.CurAmmo += AmmoInfo.MaxAmmo;
		AmmoInfo.MaxAmmo = 0;
	}
	else
	{
		AmmoInfo.MaxAmmo -= AmmoNeeded;
		AmmoInfo.CurAmmo += AmmoNeeded;
	}
}


void UPlayerAnim::AnimNotify_ReloadEnd()
{
	// 재장전할 무기 정보 배열
	TArray<FAmmoInfo> AmmoInfos = {
		{me->maxRifleAmmo, me->curRifleAmmo, 40}, // 라이플
		{me->maxSniperAmmo, me->curSniperAmmo, 5}, // 스나이퍼
		{me->maxPistolAmmo, me->curPistolAmmo, 8}, // 권총
		{me->maxM249Ammo, me->curM249Ammo, 100} // M249
	};

	// 각 무기별 재장전 처리
	for (int32 i = 0; i < me->weaponArray.Num(); i++)
	{
		if (me->weaponArray[i] == true)
		{
			ReloadAmmo(AmmoInfos[i]);
			break;
		}
	}
}


void UPlayerAnim::UpdateWeaponEquipState()
{
	// 무기 장착 상태가 변경될 때 호출되는 함수입니다.
	// 현재 장착된 무기가 권총인지 확인하고 bPistol 플래그를 설정합니다.
	if (me->weaponArray[2] == true)
	{
		bPistol = true;
	}
	else
	{
		bPistol = false;
	}
}
