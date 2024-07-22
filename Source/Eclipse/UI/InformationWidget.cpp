// Fill out your copyright notice in the Description page of Project Settings.


#include "InformationWidget.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Components/Image.h"
#include "Components/RetainerBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UInformationWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // owner 변수를 현재 월드에서 첫 번째 플레이어 캐릭터로 캐스팅하여 초기화합니다.
    owner = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    // 탄약 정보를 업데이트합니다.
    UpdateAmmo();
    UpdateAmmo_Secondary();

    // 타이머를 설정하여 0.01초마다 UpdateAmmo 함수를 호출하도록 합니다.
    FTimerHandle AmmoUpdateHandle;
    GetWorld()->GetTimerManager().SetTimer(AmmoUpdateHandle, this, &UInformationWidget::UpdateAmmo, 0.01f, true);
}

void UInformationWidget::UpdateAmmo() const
{
    // owner 변수가 유효한지 확인합니다.
    if (owner)
    {
        // 첫 번째 무기가 활성화된 경우
        if (owner->weaponArray[0] == true)
        {
            // 현재 탄약과 최대 탄약 정보를 업데이트하고, 탄약 유형을 설정합니다.
            currentAmmo->SetText(FText::AsNumber(owner->curRifleAmmo));
            maxAmmo->SetText(FText::AsNumber(owner->maxRifleAmmo));
            AmmoType->SetText(FText::FromString("5.56mm"));

            // 탄약 이미지와 아이콘의 가시성을 설정합니다.
            rifleBulletImage->SetVisibility(ESlateVisibility::Visible);
            sniperBulletImage->SetVisibility(ESlateVisibility::Hidden);
            pistolBulletImage->SetVisibility(ESlateVisibility::Hidden);
            M249BulletImage->SetVisibility(ESlateVisibility::Hidden);

            RifleIcon->SetVisibility(ESlateVisibility::Visible);
            SniperIcon->SetVisibility(ESlateVisibility::Hidden);
            PistolIcon->SetVisibility(ESlateVisibility::Hidden);
            M249Icon->SetVisibility(ESlateVisibility::Hidden);
        }
        // 두 번째 무기가 활성화된 경우
        else if (owner->weaponArray[1] == true)
        {
            // 현재 탄약과 최대 탄약 정보를 업데이트하고, 탄약 유형을 설정합니다.
            currentAmmo->SetText(FText::AsNumber(owner->curSniperAmmo));
            maxAmmo->SetText(FText::AsNumber(owner->maxSniperAmmo));
            AmmoType->SetText(FText::FromString("7.92mm"));

            // 탄약 이미지와 아이콘의 가시성을 설정합니다.
            rifleBulletImage->SetVisibility(ESlateVisibility::Hidden);
            sniperBulletImage->SetVisibility(ESlateVisibility::Visible);
            pistolBulletImage->SetVisibility(ESlateVisibility::Hidden);
            M249BulletImage->SetVisibility(ESlateVisibility::Hidden);

            RifleIcon->SetVisibility(ESlateVisibility::Hidden);
            SniperIcon->SetVisibility(ESlateVisibility::Visible);
            PistolIcon->SetVisibility(ESlateVisibility::Hidden);
            M249Icon->SetVisibility(ESlateVisibility::Hidden);
        }
        // 세 번째 무기가 활성화된 경우
        else if (owner->weaponArray[2] == true)
        {
            // 현재 탄약과 최대 탄약 정보를 업데이트하고, 탄약 유형을 설정합니다.
            currentAmmo->SetText(FText::AsNumber(owner->curPistolAmmo));
            maxAmmo->SetText(FText::AsNumber(owner->maxPistolAmmo));
            AmmoType->SetText(FText::FromString("9mm"));

            // 탄약 이미지와 아이콘의 가시성을 설정합니다.
            rifleBulletImage->SetVisibility(ESlateVisibility::Hidden);
            sniperBulletImage->SetVisibility(ESlateVisibility::Hidden);
            pistolBulletImage->SetVisibility(ESlateVisibility::Visible);
            M249BulletImage->SetVisibility(ESlateVisibility::Hidden);

            RifleIcon->SetVisibility(ESlateVisibility::Hidden);
            SniperIcon->SetVisibility(ESlateVisibility::Hidden);
            PistolIcon->SetVisibility(ESlateVisibility::Visible);
            M249Icon->SetVisibility(ESlateVisibility::Hidden);
        }
        // 네 번째 무기가 활성화된 경우
        else if (owner->weaponArray[3] == true)
        {
            // 현재 탄약과 최대 탄약 정보를 업데이트하고, 탄약 유형을 설정합니다.
            currentAmmo->SetText(FText::AsNumber(owner->curM249Ammo));
            maxAmmo->SetText(FText::AsNumber(owner->maxM249Ammo));
            AmmoType->SetText(FText::FromString("7.62mm"));

            // 탄약 이미지와 아이콘의 가시성을 설정합니다.
            rifleBulletImage->SetVisibility(ESlateVisibility::Hidden);
            sniperBulletImage->SetVisibility(ESlateVisibility::Hidden);
            pistolBulletImage->SetVisibility(ESlateVisibility::Hidden);
            M249BulletImage->SetVisibility(ESlateVisibility::Visible);

            RifleIcon->SetVisibility(ESlateVisibility::Hidden);
            SniperIcon->SetVisibility(ESlateVisibility::Hidden);
            PistolIcon->SetVisibility(ESlateVisibility::Hidden);
            M249Icon->SetVisibility(ESlateVisibility::Visible);
        }

        // 렌더링 요청을 갱신합니다.
        MaterialRetainerBox->RequestRender();
    }
}

void UInformationWidget::UpdateAmmo_Secondary() const
{
    // owner 변수가 유효한지 확인합니다.
    if (owner)
    {
        // 현재 무기 슬롯이 1번 슬롯인 경우
        if (owner->curWeaponSlotNumber == 1)
        {
            // 무기 타입에 따라 탄약 정보를 업데이트하고 이미지 및 아이콘의 가시성을 설정합니다.
            if (owner->equippedWeaponStringArray[1] == FString("Rifle"))
            {
                currentAmmo_1->SetText(FText::AsNumber(owner->curRifleAmmo));
                maxAmmo_1->SetText(FText::AsNumber(owner->maxRifleAmmo));
                AmmoType_1->SetText(FText::FromString("5.56mm"));

                rifleBulletImage_1->SetVisibility(ESlateVisibility::Visible);
                sniperBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                pistolBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                M249BulletImage_1->SetVisibility(ESlateVisibility::Hidden);

                RifleIcon_1->SetVisibility(ESlateVisibility::Visible);
                SniperIcon_1->SetVisibility(ESlateVisibility::Hidden);
                PistolIcon_1->SetVisibility(ESlateVisibility::Hidden);
                M249Icon_1->SetVisibility(ESlateVisibility::Hidden);
            }
            else if (owner->equippedWeaponStringArray[1] == FString("Sniper"))
            {
                currentAmmo_1->SetText(FText::AsNumber(owner->curSniperAmmo));
                maxAmmo_1->SetText(FText::AsNumber(owner->maxSniperAmmo));
                AmmoType_1->SetText(FText::FromString("7.92mm"));

                rifleBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                sniperBulletImage_1->SetVisibility(ESlateVisibility::Visible);
                pistolBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                M249BulletImage_1->SetVisibility(ESlateVisibility::Hidden);

                RifleIcon_1->SetVisibility(ESlateVisibility::Hidden);
                SniperIcon_1->SetVisibility(ESlateVisibility::Visible);
                PistolIcon_1->SetVisibility(ESlateVisibility::Hidden);
                M249Icon_1->SetVisibility(ESlateVisibility::Hidden);
            }
            else if (owner->equippedWeaponStringArray[1] == FString("Pistol"))
            {
                currentAmmo_1->SetText(FText::AsNumber(owner->curPistolAmmo));
                maxAmmo_1->SetText(FText::AsNumber(owner->maxPistolAmmo));
                AmmoType_1->SetText(FText::FromString("9mm"));

                rifleBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                sniperBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                pistolBulletImage_1->SetVisibility(ESlateVisibility::Visible);
                M249BulletImage_1->SetVisibility(ESlateVisibility::Hidden);

                RifleIcon_1->SetVisibility(ESlateVisibility::Hidden);
                SniperIcon_1->SetVisibility(ESlateVisibility::Hidden);
                PistolIcon_1->SetVisibility(ESlateVisibility::Visible);
                M249Icon_1->SetVisibility(ESlateVisibility::Hidden);
            }
            else if (owner->equippedWeaponStringArray[1] == FString("M249"))
            {
                currentAmmo_1->SetText(FText::AsNumber(owner->curM249Ammo));
                maxAmmo_1->SetText(FText::AsNumber(owner->maxM249Ammo));
                AmmoType_1->SetText(FText::FromString("7.62mm"));

                rifleBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                sniperBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                pistolBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                M249BulletImage_1->SetVisibility(ESlateVisibility::Visible);

                RifleIcon_1->SetVisibility(ESlateVisibility::Hidden);
                SniperIcon_1->SetVisibility(ESlateVisibility::Hidden);
                PistolIcon_1->SetVisibility(ESlateVisibility::Hidden);
                M249Icon_1->SetVisibility(ESlateVisibility::Visible);
            }
        }
        else
        {
            // 현재 무기 슬롯이 0번 슬롯인 경우
            if (owner->equippedWeaponStringArray[0] == FString("Rifle"))
            {
                currentAmmo_1->SetText(FText::AsNumber(owner->curRifleAmmo));
                maxAmmo_1->SetText(FText::AsNumber(owner->maxRifleAmmo));
                AmmoType_1->SetText(FText::FromString("5.56mm"));

                rifleBulletImage_1->SetVisibility(ESlateVisibility::Visible);
                sniperBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                pistolBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                M249BulletImage_1->SetVisibility(ESlateVisibility::Hidden);

                RifleIcon_1->SetVisibility(ESlateVisibility::Visible);
                SniperIcon_1->SetVisibility(ESlateVisibility::Hidden);
                PistolIcon_1->SetVisibility(ESlateVisibility::Hidden);
                M249Icon_1->SetVisibility(ESlateVisibility::Hidden);
            }
            else if (owner->equippedWeaponStringArray[0] == FString("Sniper"))
            {
                currentAmmo_1->SetText(FText::AsNumber(owner->curSniperAmmo));
                maxAmmo_1->SetText(FText::AsNumber(owner->maxSniperAmmo));
                AmmoType_1->SetText(FText::FromString("7.92mm"));

                rifleBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                sniperBulletImage_1->SetVisibility(ESlateVisibility::Visible);
                pistolBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                M249BulletImage_1->SetVisibility(ESlateVisibility::Hidden);

                RifleIcon_1->SetVisibility(ESlateVisibility::Hidden);
                SniperIcon_1->SetVisibility(ESlateVisibility::Visible);
                PistolIcon_1->SetVisibility(ESlateVisibility::Hidden);
                M249Icon_1->SetVisibility(ESlateVisibility::Hidden);
            }
            else if (owner->equippedWeaponStringArray[0] == FString("Pistol"))
            {
                currentAmmo_1->SetText(FText::AsNumber(owner->curPistolAmmo));
                maxAmmo_1->SetText(FText::AsNumber(owner->maxPistolAmmo));
                AmmoType_1->SetText(FText::FromString("9mm"));

                rifleBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                sniperBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                pistolBulletImage_1->SetVisibility(ESlateVisibility::Visible);
                M249BulletImage_1->SetVisibility(ESlateVisibility::Hidden);

                RifleIcon_1->SetVisibility(ESlateVisibility::Hidden);
                SniperIcon_1->SetVisibility(ESlateVisibility::Hidden);
                PistolIcon_1->SetVisibility(ESlateVisibility::Visible);
                M249Icon_1->SetVisibility(ESlateVisibility::Hidden);
            }
            else if (owner->equippedWeaponStringArray[0] == FString("M249"))
            {
                currentAmmo_1->SetText(FText::AsNumber(owner->curM249Ammo));
                maxAmmo_1->SetText(FText::AsNumber(owner->maxM249Ammo));
                AmmoType_1->SetText(FText::FromString("7.62mm"));

                rifleBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                sniperBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                pistolBulletImage_1->SetVisibility(ESlateVisibility::Hidden);
                M249BulletImage_1->SetVisibility(ESlateVisibility::Visible);

                RifleIcon_1->SetVisibility(ESlateVisibility::Hidden);
                SniperIcon_1->SetVisibility(ESlateVisibility::Hidden);
                PistolIcon_1->SetVisibility(ESlateVisibility::Hidden);
                M249Icon_1->SetVisibility(ESlateVisibility::Visible);
            }
        }
    }
}

void UInformationWidget::ChargeAmmunitionInfoWidget() const
{
    // owner 변수가 유효한지 확인합니다.
    if (owner)
    {
        // 첫 번째 무기가 활성화된 경우
        if (owner->weaponArray[0] == true)
        {
            // 충전 탄약 텍스트와 이미지의 가시성을 설정합니다.
            ChargeAmmoText->SetText(FText::FromString("+ 20"));

            rifleBulletImage_2->SetVisibility(ESlateVisibility::Visible);
            sniperBulletImage_2->SetVisibility(ESlateVisibility::Hidden);
            pistolBulletImage_2->SetVisibility(ESlateVisibility::Hidden);
            M249BulletImage_2->SetVisibility(ESlateVisibility::Hidden);
        }
        // 두 번째 무기가 활성화된 경우
        else if (owner->weaponArray[1] == true)
        {
            // 충전 탄약 텍스트와 이미지의 가시성을 설정합니다.
            ChargeAmmoText->SetText(FText::FromString("+ 4"));

            rifleBulletImage_2->SetVisibility(ESlateVisibility::Hidden);
            sniperBulletImage_2->SetVisibility(ESlateVisibility::Visible);
            pistolBulletImage_2->SetVisibility(ESlateVisibility::Hidden);
            M249BulletImage_2->SetVisibility(ESlateVisibility::Hidden);
        }
        // 세 번째 무기가 활성화된 경우
        else if (owner->weaponArray[2] == true)
        {
            // 충전 탄약 텍스트와 이미지의 가시성을 설정합니다.
            ChargeAmmoText->SetText(FText::FromString("+ 6"));

            rifleBulletImage_2->SetVisibility(ESlateVisibility::Hidden);
            sniperBulletImage_2->SetVisibility(ESlateVisibility::Hidden);
            pistolBulletImage_2->SetVisibility(ESlateVisibility::Visible);
            M249BulletImage_2->SetVisibility(ESlateVisibility::Hidden);
        }
        // 네 번째 무기가 활성화된 경우
        else if (owner->weaponArray[3] == true)
        {
            // 충전 탄약 텍스트와 이미지의 가시성을 설정합니다.
            ChargeAmmoText->SetText(FText::FromString("+ 30"));

            rifleBulletImage_2->SetVisibility(ESlateVisibility::Hidden);
            sniperBulletImage_2->SetVisibility(ESlateVisibility::Hidden);
            pistolBulletImage_2->SetVisibility(ESlateVisibility::Hidden);
            M249BulletImage_2->SetVisibility(ESlateVisibility::Visible);
        }
    }
}
