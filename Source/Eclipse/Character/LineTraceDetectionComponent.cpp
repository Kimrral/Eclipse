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
	if (Owner->bEnding)
	{
		return;
	}

	TMap<UClass*, int32> ActorWidgetMap = {
		{ARifleActor::StaticClass(), 0},
		{ASniperActor::StaticClass(), 1},
		{APistolActor::StaticClass(), 2},
		{AM249Actor::StaticClass(), 3},
		{ARifleMagActor::StaticClass(), 6},
		{ASniperMagActor::StaticClass(), 7},
		{APistolMagActor::StaticClass(), 8},
		{AM249MagActor::StaticClass(), 9},
		{AGoggleActor::StaticClass(), 10},
		{AHelmetActor::StaticClass(), 11},
		{AHeadsetActor::StaticClass(), 12},
		{AMaskActor::StaticClass(), 13},
		{AArmorActor::StaticClass(), 14},
		{AMedKitActor::StaticClass(), 15},
		{AFirstAidKitActor::StaticClass(), 20}
	};

	if (Owner->IsLocallyControlled())
	{
		if (TickOverlapBoolean)
		{
			FVector StartLoc = Owner->FollowCamera->GetComponentLocation();
			FVector EndLoc = StartLoc + Owner->FollowCamera->GetForwardVector() * 500.0f;

			FHitResult ActorHitResult;
			if (GetWorld()->LineTraceSingleByChannel(ActorHitResult, StartLoc, EndLoc, ECC_Visibility))
			{
				AActor* HitActor = ActorHitResult.GetActor();

				if (!TickOverlapBoolean && HitActor != nullptr)
				{
					if (int32* WidgetIndex = ActorWidgetMap.Find(HitActor->GetClass()))
					{
						TickOverlapBoolean = true;

						// 외곽선 활성화
						if (UStaticMeshComponent* MeshComp = HitActor->FindComponentByClass<UStaticMeshComponent>())
						{
							MeshComp->SetRenderCustomDepth(true);
						}

						// 위젯 스위칭 및 초기화
						Owner->infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(*WidgetIndex);
						Owner->infoWidgetUI->weaponHoldPercent = 0;
						Owner->infoWidgetUI->AddToViewport();
					}
				}
			}
		}
	}
	else
	{
		TickOverlapBoolean = false;
		Owner->infoWidgetUI->RemoveFromParent();

		// Sphere Collision Check
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Owner);
		FVector Center = Owner->GetActorLocation();
		TArray<FOverlapResult> HitResults;

		if (GetWorld()->OverlapMultiByChannel(HitResults, Center, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(500), Params))
		{
			for (const FOverlapResult& HitResult : HitResults)
			{
				// 공통 로직으로 외곽선 해제
				if (UStaticMeshComponent* MeshComp = HitResult.GetActor()->FindComponentByClass<UStaticMeshComponent>())
				{
					MeshComp->SetRenderCustomDepth(false);
				}
			}
		}
	}
}
