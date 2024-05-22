// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Character/LineTraceDetectionComponent.h"

#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Eclipse/Item/AdrenalineSyringe.h"
#include "Eclipse/Item/FirstAidKitActor.h"
#include "Eclipse/Item/GoggleActor.h"
#include "Eclipse/Item/HackingConsole.h"
#include "Eclipse/Item/HeadsetActor.h"
#include "Eclipse/Item/HelmetActor.h"
#include "Eclipse/Item/M249AmmoPack.h"
#include "Eclipse/Item/M249MagActor.h"
#include "Eclipse/Item/MaskActor.h"
#include "Eclipse/Item/MedKitActor.h"
#include "Eclipse/Item/MilitaryDevice.h"
#include "Eclipse/Item/MilitaryLaptop.h"
#include "Eclipse/Item/PistolAmmoPack.h"
#include "Eclipse/Item/PistolMagActor.h"
#include "Eclipse/Item/PoisonOfSpider.h"
#include "Eclipse/Item/RifleAmmoPack.h"
#include "Eclipse/Item/RifleMagActor.h"
#include "Eclipse/Item/SniperAmmoPack.h"
#include "Eclipse/Item/SniperMagActor.h"
#include "Eclipse/Prop/MissionChecker.h"
#include "Eclipse/Prop/QuitGameActor.h"
#include "Eclipse/Prop/StageBoard.h"
#include "Eclipse/Prop/Stash.h"
#include "Eclipse/Prop/Trader.h"
#include "Eclipse/UI/WeaponInfoWidget.h"

// Sets default values for this component's properties
ULineTraceDetectionComponent::ULineTraceDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void ULineTraceDetectionComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<APlayerCharacter>(GetOwner());
}

// Called every frame
void ULineTraceDetectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LineTraceDetection();
}


void ULineTraceDetectionComponent::LineTraceDetection()
{
	if(Owner->bEnding)
	{
		return;
	}
	if (Owner->IsLocallyControlled())
	{		
		FVector StartLoc = Owner->FollowCamera->GetComponentLocation();
		FVector EndLoc = StartLoc + Owner->FollowCamera->GetForwardVector() * 500.0f;
		// 무기 액터 탐지 라인 트레이스
		if (FHitResult ActorHitResult; GetWorld()->LineTraceSingleByChannel(ActorHitResult, StartLoc, EndLoc, ECC_Visibility))
		{
			// 무기 액터 캐스팅
			rifleActor = Cast<ARifleActor>(ActorHitResult.GetActor());
			sniperActor = Cast<ASniperActor>(ActorHitResult.GetActor());
			pistolActor = Cast<APistolActor>(ActorHitResult.GetActor());
			m249Actor = Cast<AM249Actor>(ActorHitResult.GetActor());
			MissionChecker = Cast<AMissionChecker>(ActorHitResult.GetActor());
			PickableItemActor = Cast<APickableActor>(ActorHitResult.GetActor());
			StageBoard = Cast<AStageBoard>(ActorHitResult.GetActor());
			Stash = Cast<AStash>(ActorHitResult.GetActor());
			Trader = Cast<ATrader>(ActorHitResult.GetActor());
			QuitGameActor = Cast<AQuitGameActor>(ActorHitResult.GetActor());
			DeadPlayerContainer = Cast<ADeadPlayerContainer>(ActorHitResult.GetActor());

			// 라이플 탐지
			if (rifleActor)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					rifleActor->weaponMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(0);
					// Radial Slider Value 초기화
					Owner->infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					Owner->infoWidgetUI->AddToViewport();
				}
			}
			else if (sniperActor)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					sniperActor->weaponMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(1);
					// Radial Slider Value 초기화
					Owner->infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					Owner->infoWidgetUI->AddToViewport();
				}
			}
			else if (pistolActor)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					pistolActor->weaponMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(2);
					// Radial Slider Value 초기화
					Owner->infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					Owner->infoWidgetUI->AddToViewport();
				}
			}
			else if (m249Actor)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					m249Actor->weaponMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(3);
					// Radial Slider Value 초기화
					Owner->infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					Owner->infoWidgetUI->AddToViewport();
				}
			}
			else if (PickableItemActor)
			{
				HackingConsole = Cast<AHackingConsole>(PickableItemActor);
				RifleMagActor = Cast<ARifleMagActor>(PickableItemActor);
				SniperMagActor = Cast<ASniperMagActor>(PickableItemActor);
				PistolMagActor = Cast<APistolMagActor>(PickableItemActor);
				M249MagActor = Cast<AM249MagActor>(PickableItemActor);
				GoggleActor = Cast<AGoggleActor>(PickableItemActor);
				MaskActor = Cast<AMaskActor>(PickableItemActor);
				HelmetActor = Cast<AHelmetActor>(PickableItemActor);
				HeadsetActor = Cast<AHeadsetActor>(PickableItemActor);
				ArmorActor = Cast<AArmorActor>(PickableItemActor);
				MedKitActor = Cast<AMedKitActor>(PickableItemActor);
				MilitaryLaptop = Cast<AMilitaryLaptop>(PickableItemActor);
				MilitaryDevice = Cast<AMilitaryDevice>(PickableItemActor);
				RifleAmmoPack = Cast<ARifleAmmoPack>(PickableItemActor);
				SniperAmmoPack = Cast<ASniperAmmoPack>(PickableItemActor);
				PistolAmmoPack = Cast<APistolAmmoPack>(PickableItemActor);
				M249AmmoPack = Cast<AM249AmmoPack>(PickableItemActor);
				AdrenalineSyringe = Cast<AAdrenalineSyringe>(PickableItemActor);
				PoisonOfSpider = Cast<APoisonOfSpider>(PickableItemActor);
				FirstAidKitActor = Cast<AFirstAidKitActor>(PickableItemActor);
				if (RifleMagActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						RifleMagActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(6);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (SniperMagActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						SniperMagActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(7);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (PistolMagActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						PistolMagActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(8);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (M249MagActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						M249MagActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(9);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (GoggleActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						GoggleActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(10);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (HelmetActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						HelmetActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(11);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (HeadsetActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						HeadsetActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(12);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (MaskActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						MaskActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(13);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (ArmorActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						ArmorActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(14);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (MedKitActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						MedKitActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(15);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (FirstAidKitActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						FirstAidKitActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(20);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (AdrenalineSyringe)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						AdrenalineSyringe->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(23);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (PoisonOfSpider)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						PoisonOfSpider->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(24);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (MilitaryLaptop)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						MilitaryLaptop->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(21);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (MilitaryDevice)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						MilitaryDevice->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(22);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (RifleAmmoPack)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						RifleAmmoPack->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(26);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (SniperAmmoPack)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						SniperAmmoPack->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(27);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (PistolAmmoPack)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						PistolAmmoPack->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(28);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (M249AmmoPack)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						M249AmmoPack->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(29);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
				else if (HackingConsole)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						HackingConsole->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(4);
						// Radial Slider Value 초기화
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						Owner->infoWidgetUI->AddToViewport();
					}
				}
			}

			else if (MissionChecker)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					MissionChecker->checkerMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(5);
					// Radial Slider Value 초기화
					Owner->infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					Owner->infoWidgetUI->AddToViewport();
				}
			}
			else if (StageBoard)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					StageBoard->boardMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(16);
					// Radial Slider Value 초기화
					Owner->infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					Owner->infoWidgetUI->AddToViewport();
				}
			}
			else if (Stash)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					Stash->stashMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(17);
					// Radial Slider Value 초기화
					Owner->infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					Owner->infoWidgetUI->AddToViewport();
				}
			}
			else if (Trader)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					Trader->TraderCharacterMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(25);
					// Radial Slider Value 초기화
					Owner->infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					Owner->infoWidgetUI->AddToViewport();
				}
			}
			else if (QuitGameActor)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					QuitGameActor->quitGameMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(18);
					// Radial Slider Value 초기화
					Owner->infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					Owner->infoWidgetUI->AddToViewport();
				}
			}
			else if (DeadPlayerContainer)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					DeadPlayerContainer->DeadBodyMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(19);
					// Radial Slider Value 초기화
					Owner->infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					Owner->infoWidgetUI->AddToViewport();
				}
			}
			else
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == true)
				{
					TickOverlapBoolean = false;
					// 무기 액터 정보 위젯 파괴
					Owner->infoWidgetUI->RemoveFromParent();
					// 중심점
					FVector Center = Owner->GetActorLocation();
					// 충돌체크(구충돌)
					// 충돌한 물체를 기억할 배열					;
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(Owner);
					// End Overlap 시점에 호출되는 Overlap Multi
					if (TArray<FOverlapResult> HitObj; GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(500), Params))
					{
						// 충돌 배열 순회
						for (int i = 0; i < HitObj.Num(); ++i)
						{
							// 무기 액터 캐스팅
							rifleActor = Cast<ARifleActor>(HitObj[i].GetActor());
							sniperActor = Cast<ASniperActor>(HitObj[i].GetActor());
							pistolActor = Cast<APistolActor>(HitObj[i].GetActor());
							m249Actor = Cast<AM249Actor>(HitObj[i].GetActor());
							MissionChecker = Cast<AMissionChecker>(HitObj[i].GetActor());
							HackingConsole = Cast<AHackingConsole>(HitObj[i].GetActor());
							RifleMagActor = Cast<ARifleMagActor>(HitObj[i].GetActor());
							SniperMagActor = Cast<ASniperMagActor>(HitObj[i].GetActor());
							PistolMagActor = Cast<APistolMagActor>(HitObj[i].GetActor());
							M249MagActor = Cast<AM249MagActor>(HitObj[i].GetActor());
							GoggleActor = Cast<AGoggleActor>(HitObj[i].GetActor());
							MaskActor = Cast<AMaskActor>(HitObj[i].GetActor());
							HelmetActor = Cast<AHelmetActor>(HitObj[i].GetActor());
							HeadsetActor = Cast<AHeadsetActor>(HitObj[i].GetActor());
							ArmorActor = Cast<AArmorActor>(HitObj[i].GetActor());
							MedKitActor = Cast<AMedKitActor>(HitObj[i].GetActor());
							FirstAidKitActor = Cast<AFirstAidKitActor>(HitObj[i].GetActor());
							AdrenalineSyringe = Cast<AAdrenalineSyringe>(HitObj[i].GetActor());
							PoisonOfSpider = Cast<APoisonOfSpider>(HitObj[i].GetActor());
							MilitaryLaptop = Cast<AMilitaryLaptop>(HitObj[i].GetActor());
							MilitaryDevice = Cast<AMilitaryDevice>(HitObj[i].GetActor());
							RifleAmmoPack = Cast<ARifleAmmoPack>(HitObj[i].GetActor());
							SniperAmmoPack = Cast<ASniperAmmoPack>(HitObj[i].GetActor());
							PistolAmmoPack = Cast<APistolAmmoPack>(HitObj[i].GetActor());
							M249AmmoPack = Cast<AM249AmmoPack>(HitObj[i].GetActor());
							StageBoard = Cast<AStageBoard>(HitObj[i].GetActor());
							Stash = Cast<AStash>(HitObj[i].GetActor());
							Trader = Cast<ATrader>(HitObj[i].GetActor());
							QuitGameActor = Cast<AQuitGameActor>(HitObj[i].GetActor());
							DeadPlayerContainer = Cast<ADeadPlayerContainer>(HitObj[i].GetActor());

							if (rifleActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								rifleActor->weaponMesh->SetRenderCustomDepth(false);
							}
							else if (sniperActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								sniperActor->weaponMesh->SetRenderCustomDepth(false);
							}
							else if (pistolActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								pistolActor->weaponMesh->SetRenderCustomDepth(false);
							}
							else if (m249Actor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								m249Actor->weaponMesh->SetRenderCustomDepth(false);
							}
							else if (HackingConsole)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								HackingConsole->RootMesh->SetRenderCustomDepth(false);
							}
							else if (MissionChecker)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								MissionChecker->checkerMesh->SetRenderCustomDepth(false);
							}
							else if (RifleMagActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								RifleMagActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (SniperMagActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								SniperMagActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (PistolMagActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								PistolMagActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (M249MagActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								M249MagActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (GoggleActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								GoggleActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (HelmetActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								HelmetActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (HeadsetActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								HeadsetActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (MaskActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								MaskActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (ArmorActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								ArmorActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (MedKitActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								MedKitActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (FirstAidKitActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								FirstAidKitActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (AdrenalineSyringe)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								AdrenalineSyringe->RootMesh->SetRenderCustomDepth(false);
							}
							else if (PoisonOfSpider)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								PoisonOfSpider->RootMesh->SetRenderCustomDepth(false);
							}
							else if (MilitaryLaptop)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								MilitaryLaptop->RootMesh->SetRenderCustomDepth(false);
							}
							else if (MilitaryDevice)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								MilitaryDevice->RootMesh->SetRenderCustomDepth(false);
							}
							else if (RifleAmmoPack)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								RifleAmmoPack->RootMesh->SetRenderCustomDepth(false);
							}
							else if (SniperAmmoPack)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								SniperAmmoPack->RootMesh->SetRenderCustomDepth(false);
							}
							else if (PistolAmmoPack)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								PistolAmmoPack->RootMesh->SetRenderCustomDepth(false);
							}
							else if (M249AmmoPack)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								M249AmmoPack->RootMesh->SetRenderCustomDepth(false);
							}
							else if (StageBoard)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								StageBoard->boardMesh->SetRenderCustomDepth(false);
							}
							else if (Stash)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								Stash->stashMesh->SetRenderCustomDepth(false);
							}
							else if (Trader)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								Trader->TraderCharacterMesh->SetRenderCustomDepth(false);
							}
							else if (QuitGameActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								QuitGameActor->quitGameMesh->SetRenderCustomDepth(false);
							}
							else if (DeadPlayerContainer)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								DeadPlayerContainer->DeadBodyMesh->SetRenderCustomDepth(false);
							}
						}
					}
				}
			}
		}
		else
		{
		}
	}
}
