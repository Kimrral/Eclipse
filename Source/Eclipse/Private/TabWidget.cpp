// Fill out your copyright notice in the Description page of Project Settings.


#include "TabWidget.h"

#include "ArmorActor.h"
#include "GoggleActor.h"
#include "HackingConsole.h"
#include "HeadsetActor.h"
#include "HelmetActor.h"
#include "M249MagActor.h"
#include "MaskActor.h"
#include "MedKitActor.h"
#include "PistolMagActor.h"
#include "PlayerCharacter.h"
#include "RifleMagActor.h"
#include "SniperMagActor.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UTabWidget::SetTabWidget()
{
	if(player)
	{
		CurHPText->SetText(FText::AsNumber(player->curHP));
		MaxHPText->SetText(FText::AsNumber(player->maxHP));
		MaxBulletText1->SetText(FText::AsNumber(player->maxRifleAmmo));
		MaxBulletText2->SetText(FText::AsNumber(player->maxSniperAmmo));
		MaxBulletText3->SetText(FText::AsNumber(player->maxPistolAmmo));
		MaxBulletText4->SetText(FText::AsNumber(player->maxM249Ammo));
		
		if(player->equippedWeaponStringArray[0]==FString("Rifle"))
		{
			CurWeaponImage1->SetVisibility(ESlateVisibility::Visible);
			CurWeaponImage1_1->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage1_2->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage1_3->SetVisibility(ESlateVisibility::Hidden);
			
			CurBulletImage1->SetVisibility(ESlateVisibility::Visible);
			CurBulletImage1_1->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage1_2->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage1_3->SetVisibility(ESlateVisibility::Hidden);
	
			WeaponText1->SetText(FText::FromString("ASSAULT RIFLE"));
			BulletText1->SetText(FText::FromString("5.56mm"));
			CurBulletText1->SetText(FText::AsNumber(player->curRifleAmmo));
	
		}
		else if(player->equippedWeaponStringArray[0]==FString("Sniper"))
		{
			CurWeaponImage1->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage1_1->SetVisibility(ESlateVisibility::Visible);
			CurWeaponImage1_2->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage1_3->SetVisibility(ESlateVisibility::Hidden);
	
			CurBulletImage1->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage1_1->SetVisibility(ESlateVisibility::Visible);
			CurBulletImage1_2->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage1_3->SetVisibility(ESlateVisibility::Hidden);
	
			WeaponText1->SetText(FText::FromString("SNIPER RIFLE"));
			BulletText1->SetText(FText::FromString("7.92mm"));
			CurBulletText1->SetText(FText::AsNumber(player->curSniperAmmo));
	
		}
		else if(player->equippedWeaponStringArray[0]==FString("Pistol"))
		{
			CurWeaponImage1->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage1_1->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage1_2->SetVisibility(ESlateVisibility::Visible);
			CurWeaponImage1_3->SetVisibility(ESlateVisibility::Hidden);
	
			CurBulletImage1->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage1_1->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage1_2->SetVisibility(ESlateVisibility::Visible);
			CurBulletImage1_3->SetVisibility(ESlateVisibility::Hidden);
	
			WeaponText1->SetText(FText::FromString("PISTOL"));
			BulletText1->SetText(FText::FromString("9mm"));
			CurBulletText1->SetText(FText::AsNumber(player->curPistolAmmo));
	
		}
		else if(player->equippedWeaponStringArray[0]==FString("M249"))
		{
			CurWeaponImage1->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage1_1->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage1_2->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage1_3->SetVisibility(ESlateVisibility::Visible);
	
			CurBulletImage1->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage1_1->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage1_2->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage1_3->SetVisibility(ESlateVisibility::Visible);
	
			WeaponText1->SetText(FText::FromString("M249"));
			BulletText1->SetText(FText::FromString("7.62mm"));
			CurBulletText1->SetText(FText::AsNumber(player->curM249Ammo));
	
		}
	
		
		if(player->equippedWeaponStringArray[1]==FString("Rifle"))
		{
	
			CurWeaponImage2->SetVisibility(ESlateVisibility::Visible);
			CurWeaponImage2_1->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage2_2->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage2_3->SetVisibility(ESlateVisibility::Hidden);
			
			CurBulletImage2->SetVisibility(ESlateVisibility::Visible);
			CurBulletImage2_1->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage2_2->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage2_3->SetVisibility(ESlateVisibility::Hidden);
	
			WeaponText2->SetText(FText::FromString("ASSAULT RIFLE"));
			BulletText2->SetText(FText::FromString("5.56mm"));
			CurBulletText2->SetText(FText::AsNumber(player->curRifleAmmo));
	
		}
		else if(player->equippedWeaponStringArray[1]==FString("Sniper"))
		{
			CurWeaponImage2->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage2_1->SetVisibility(ESlateVisibility::Visible);
			CurWeaponImage2_2->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage2_3->SetVisibility(ESlateVisibility::Hidden);
			
			CurBulletImage2->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage2_1->SetVisibility(ESlateVisibility::Visible);
			CurBulletImage2_2->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage2_3->SetVisibility(ESlateVisibility::Hidden);
	
			WeaponText2->SetText(FText::FromString("SNIPER RIFLE"));
			BulletText2->SetText(FText::FromString("7.92mm"));
			CurBulletText2->SetText(FText::AsNumber(player->curSniperAmmo));
		}
		else if(player->equippedWeaponStringArray[1]==FString("Pistol"))
		{
			CurWeaponImage2->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage2_1->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage2_2->SetVisibility(ESlateVisibility::Visible);
			CurWeaponImage2_3->SetVisibility(ESlateVisibility::Hidden);
			
			CurBulletImage2->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage2_1->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage2_2->SetVisibility(ESlateVisibility::Visible);
			CurBulletImage2_3->SetVisibility(ESlateVisibility::Hidden);
	
			WeaponText2->SetText(FText::FromString("PISTOL"));
			BulletText2->SetText(FText::FromString("9mm"));
			CurBulletText2->SetText(FText::AsNumber(player->curPistolAmmo));
	
		}
		else if(player->equippedWeaponStringArray[1]==FString("M249"))
		{
			CurWeaponImage2->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage2_1->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage2_2->SetVisibility(ESlateVisibility::Hidden);
			CurWeaponImage2_3->SetVisibility(ESlateVisibility::Visible);
			
			CurBulletImage2->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage2_1->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage2_2->SetVisibility(ESlateVisibility::Hidden);
			CurBulletImage2_3->SetVisibility(ESlateVisibility::Visible);
	
			WeaponText2->SetText(FText::FromString("M249"));
			BulletText2->SetText(FText::FromString("7.62mm"));
			CurBulletText2->SetText(FText::AsNumber(player->curM249Ammo));
		}
	}
}

