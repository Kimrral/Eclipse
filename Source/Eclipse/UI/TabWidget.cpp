// Fill out your copyright notice in the Description page of Project Settings.


#include "TabWidget.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UTabWidget::SetTabWidget()
{
    // 플레이어가 유효한지 확인
    if (player)
    {
        // 플레이어의 현재 HP와 최대 HP를 텍스트로 설정
        CurHPText->SetText(FText::AsNumber(player->Stat->GetCurrentHp()));
        MaxHPText->SetText(FText::AsNumber(player->Stat->GetMaxHp()));
        
        // 각 무기 종류별 최대 탄약 수를 텍스트로 설정
        MaxBulletText1->SetText(FText::AsNumber(player->maxRifleAmmo));
        MaxBulletText2->SetText(FText::AsNumber(player->maxSniperAmmo));
        MaxBulletText3->SetText(FText::AsNumber(player->maxPistolAmmo));
        MaxBulletText4->SetText(FText::AsNumber(player->maxM249Ammo));
        
        // 첫 번째 슬롯에 장착된 무기 처리
        if (player->equippedWeaponStringArray.IsValidIndex(0))
        {
            // 무기가 비어있는 경우
            if (player->equippedWeaponStringArray[0] == FString("Empty"))
            {
                // 무기 및 탄약 이미지 감추기
                CurWeaponImage1->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage1_1->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage1_2->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage1_3->SetVisibility(ESlateVisibility::Hidden);

                CurBulletImage1->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage1_1->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage1_2->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage1_3->SetVisibility(ESlateVisibility::Hidden);

                // 무기 텍스트 설정
                WeaponText1->SetText(FText::FromString("Empty"));
                BulletText1->SetText(FText::FromString(" "));
                CurBulletText1->SetText(FText::FromString(" "));
            }
            // 무기가 라이플인 경우
            else if (player->equippedWeaponStringArray[0] == FString("Rifle"))
            {
                // 라이플 이미지 보이기
                CurWeaponImage1->SetVisibility(ESlateVisibility::Visible);
                CurWeaponImage1_1->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage1_2->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage1_3->SetVisibility(ESlateVisibility::Hidden);

                // 라이플 탄약 이미지 보이기
                CurBulletImage1->SetVisibility(ESlateVisibility::Visible);
                CurBulletImage1_1->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage1_2->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage1_3->SetVisibility(ESlateVisibility::Hidden);

                // 무기 텍스트 설정
                WeaponText1->SetText(FText::FromString("ASSAULT RIFLE"));
                BulletText1->SetText(FText::FromString("5.56mm"));
                CurBulletText1->SetText(FText::AsNumber(player->curRifleAmmo));
            }
            // 무기가 저격총인 경우
            else if (player->equippedWeaponStringArray[0] == FString("Sniper"))
            {
                // 저격총 이미지 보이기
                CurWeaponImage1->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage1_1->SetVisibility(ESlateVisibility::Visible);
                CurWeaponImage1_2->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage1_3->SetVisibility(ESlateVisibility::Hidden);

                // 저격총 탄약 이미지 보이기
                CurBulletImage1->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage1_1->SetVisibility(ESlateVisibility::Visible);
                CurBulletImage1_2->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage1_3->SetVisibility(ESlateVisibility::Hidden);

                // 무기 텍스트 설정
                WeaponText1->SetText(FText::FromString("SNIPER RIFLE"));
                BulletText1->SetText(FText::FromString("7.92mm"));
                CurBulletText1->SetText(FText::AsNumber(player->curSniperAmmo));
            }
            // 무기가 권총인 경우
            else if (player->equippedWeaponStringArray[0] == FString("Pistol"))
            {
                // 권총 이미지 보이기
                CurWeaponImage1->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage1_1->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage1_2->SetVisibility(ESlateVisibility::Visible);
                CurWeaponImage1_3->SetVisibility(ESlateVisibility::Hidden);

                // 권총 탄약 이미지 보이기
                CurBulletImage1->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage1_1->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage1_2->SetVisibility(ESlateVisibility::Visible);
                CurBulletImage1_3->SetVisibility(ESlateVisibility::Hidden);

                // 무기 텍스트 설정
                WeaponText1->SetText(FText::FromString("PISTOL"));
                BulletText1->SetText(FText::FromString("9mm"));
                CurBulletText1->SetText(FText::AsNumber(player->curPistolAmmo));
            }
            // 무기가 M249인 경우
            else if (player->equippedWeaponStringArray[0] == FString("M249"))
            {
                // M249 이미지 보이기
                CurWeaponImage1->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage1_1->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage1_2->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage1_3->SetVisibility(ESlateVisibility::Visible);

                // M249 탄약 이미지 보이기
                CurBulletImage1->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage1_1->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage1_2->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage1_3->SetVisibility(ESlateVisibility::Visible);

                // 무기 텍스트 설정
                WeaponText1->SetText(FText::FromString("M249"));
                BulletText1->SetText(FText::FromString("7.62mm"));
                CurBulletText1->SetText(FText::AsNumber(player->curM249Ammo));
            }
        }

        // 두 번째 슬롯에 장착된 무기 처리
        if (player->equippedWeaponStringArray.IsValidIndex(1))
        {
            // 무기가 비어있는 경우
            if (player->equippedWeaponStringArray[1] == FString("Empty"))
            {
                // 무기 및 탄약 이미지 감추기
                CurWeaponImage2->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage2_1->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage2_2->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage2_3->SetVisibility(ESlateVisibility::Hidden);

                CurBulletImage2->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage2_1->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage2_2->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage2_3->SetVisibility(ESlateVisibility::Hidden);

                // 무기 텍스트 설정
                WeaponText2->SetText(FText::FromString("Empty"));
                BulletText2->SetText(FText::FromString(" "));
                CurBulletText2->SetText(FText::FromString(" "));
            }
            // 무기가 라이플인 경우
            else if (player->equippedWeaponStringArray[1] == FString("Rifle"))
            {
                // 라이플 이미지 보이기
                CurWeaponImage2->SetVisibility(ESlateVisibility::Visible);
                CurWeaponImage2_1->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage2_2->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage2_3->SetVisibility(ESlateVisibility::Hidden);

                // 라이플 탄약 이미지 보이기
                CurBulletImage2->SetVisibility(ESlateVisibility::Visible);
                CurBulletImage2_1->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage2_2->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage2_3->SetVisibility(ESlateVisibility::Hidden);

                // 무기 텍스트 설정
                WeaponText2->SetText(FText::FromString("ASSAULT RIFLE"));
                BulletText2->SetText(FText::FromString("5.56mm"));
                CurBulletText2->SetText(FText::AsNumber(player->curRifleAmmo));
            }
            // 무기가 저격총인 경우
            else if (player->equippedWeaponStringArray[1] == FString("Sniper"))
            {
                // 저격총 이미지 보이기
                CurWeaponImage2->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage2_1->SetVisibility(ESlateVisibility::Visible);
                CurWeaponImage2_2->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage2_3->SetVisibility(ESlateVisibility::Hidden);

                // 저격총 탄약 이미지 보이기
                CurBulletImage2->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage2_1->SetVisibility(ESlateVisibility::Visible);
                CurBulletImage2_2->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage2_3->SetVisibility(ESlateVisibility::Hidden);

                // 무기 텍스트 설정
                WeaponText2->SetText(FText::FromString("SNIPER RIFLE"));
                BulletText2->SetText(FText::FromString("7.92mm"));
                CurBulletText2->SetText(FText::AsNumber(player->curSniperAmmo));
            }
            // 무기가 권총인 경우
            else if (player->equippedWeaponStringArray[1] == FString("Pistol"))
            {
                // 권총 이미지 보이기
                CurWeaponImage2->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage2_1->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage2_2->SetVisibility(ESlateVisibility::Visible);
                CurWeaponImage2_3->SetVisibility(ESlateVisibility::Hidden);

                // 권총 탄약 이미지 보이기
                CurBulletImage2->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage2_1->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage2_2->SetVisibility(ESlateVisibility::Visible);
                CurBulletImage2_3->SetVisibility(ESlateVisibility::Hidden);

                // 무기 텍스트 설정
                WeaponText2->SetText(FText::FromString("PISTOL"));
                BulletText2->SetText(FText::FromString("9mm"));
                CurBulletText2->SetText(FText::AsNumber(player->curPistolAmmo));
            }
            // 무기가 M249인 경우
            else if (player->equippedWeaponStringArray[1] == FString("M249"))
            {
                // M249 이미지 보이기
                CurWeaponImage2->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage2_1->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage2_2->SetVisibility(ESlateVisibility::Hidden);
                CurWeaponImage2_3->SetVisibility(ESlateVisibility::Visible);

                // M249 탄약 이미지 보이기
                CurBulletImage2->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage2_1->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage2_2->SetVisibility(ESlateVisibility::Hidden);
                CurBulletImage2_3->SetVisibility(ESlateVisibility::Visible);

                // 무기 텍스트 설정
                WeaponText2->SetText(FText::FromString("M249"));
                BulletText2->SetText(FText::FromString("7.62mm"));
                CurBulletText2->SetText(FText::AsNumber(player->curM249Ammo));
            }
        }
    }
}

// 탄약 정보를 업데이트하는 함수
void UTabWidget::UpdateAmmunition()
{
    // 각 무기 종류별 최대 탄약 수를 텍스트로 설정
    MaxBulletText1->SetText(FText::AsNumber(player->maxRifleAmmo));
    MaxBulletText2->SetText(FText::AsNumber(player->maxSniperAmmo));
    MaxBulletText3->SetText(FText::AsNumber(player->maxPistolAmmo));
    MaxBulletText4->SetText(FText::AsNumber(player->maxM249Ammo));
}

// HP 정보를 업데이트하는 함수
void UTabWidget::UpdateHealthPoint()
{
    // 플레이어가 유효한지 확인
    if (player)
    {
        // 플레이어의 현재 HP와 최대 HP를 텍스트로 설정
        CurHPText->SetText(FText::AsNumber(player->Stat->GetCurrentHp()));
        MaxHPText->SetText(FText::AsNumber(player->Stat->GetMaxHp()));
    }
}

TArray<FPlayerInventoryStruct> UTabWidget::GetInventoryData()
{
    return {};
}

TArray<int32> UTabWidget::GetInventoryStacks()
{
    return {};
}
