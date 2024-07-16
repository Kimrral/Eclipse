// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnim.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

void UPlayerAnim::AnimNotify_ReloadEnd() const
{
    // 재장전 애니메이션이 끝날 때 호출되는 함수입니다.

    // 현재 장착된 무기에 따라 적절한 총알을 재장전합니다.
    if (me->weaponArray[0] == true) // 라이플이 장착된 경우
    {
        if (me->maxRifleAmmo < (40 - me->curRifleAmmo))
        {
            // 현재 라이플 총알보다 최대 총알이 적으면 모든 총알을 재장전합니다.
            me->curRifleAmmo += me->maxRifleAmmo;
            me->maxRifleAmmo = 0;
        }
        else
        {
            // 현재 라이플 총알을 최대 총알 수만큼 재장전합니다.
            me->maxRifleAmmo -= (40 - me->curRifleAmmo);
            me->curRifleAmmo += (40 - me->curRifleAmmo);
        }
    }
    else if (me->weaponArray[1] == true) // 스나이퍼가 장착된 경우
    {
        if (me->maxSniperAmmo < (5 - me->curSniperAmmo))
        {
            // 현재 스나이퍼 총알보다 최대 총알이 적으면 모든 총알을 재장전합니다.
            me->curSniperAmmo += me->maxSniperAmmo;
            me->maxSniperAmmo = 0;
        }
        else
        {
            // 현재 스나이퍼 총알을 최대 총알 수만큼 재장전합니다.
            me->maxSniperAmmo -= (5 - me->curSniperAmmo);
            me->curSniperAmmo += (5 - me->curSniperAmmo);
        }
    }
    else if (me->weaponArray[2] == true) // 권총이 장착된 경우
    {
        if (me->maxPistolAmmo < (8 - me->curPistolAmmo))
        {
            // 현재 권총 총알보다 최대 총알이 적으면 모든 총알을 재장전합니다.
            me->curPistolAmmo += me->maxPistolAmmo;
            me->maxPistolAmmo = 0;
        }
        else
        {
            // 현재 권총 총알을 최대 총알 수만큼 재장전합니다.
            me->maxPistolAmmo -= (8 - me->curPistolAmmo);
            me->curPistolAmmo += (8 - me->curPistolAmmo);
        }
    }
    else if (me->weaponArray[3] == true) // M249가 장착된 경우
    {
        if (me->maxM249Ammo < (100 - me->curM249Ammo))
        {
            // 현재 M249 총알보다 최대 총알이 적으면 모든 총알을 재장전합니다.
            me->curM249Ammo += me->maxM249Ammo;
            me->maxM249Ammo = 0;
        }
        else
        {
            // 현재 M249 총알을 최대 총알 수만큼 재장전합니다.
            me->maxM249Ammo -= (100 - me->curM249Ammo);
            me->curM249Ammo += (100 - me->curM249Ammo);
        }
    }
}

void UPlayerAnim::UpdateWeaponEquipState()
{
    // 무기 장착 상태가 변경될 때 호출되는 함수입니다.
    // 현재 장착된 무기가 권총인지 확인하고 bPistol 플래그를 설정합니다.
    if(me->weaponArray[2] == true)
    {
        bPistol = true;
    }
    else
    {
        bPistol = false;
    }
}
