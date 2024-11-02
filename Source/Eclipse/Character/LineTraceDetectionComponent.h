// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LineTraceDetectionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECLIPSE_API ULineTraceDetectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULineTraceDetectionComponent();

	UFUNCTION()
	void LineTraceDetection();

	UPROPERTY()
	class APlayerCharacter* Owner;

	UPROPERTY()
	class APickableActor* PickableItemActor;

	UPROPERTY()
	class AMaskActor* MaskActor;

	UPROPERTY()
	class AHelmetActor* HelmetActor;

	UPROPERTY()
	class AHeadsetActor* HeadsetActor;

	UPROPERTY()
	class AGoggleActor* GoggleActor;

	UPROPERTY()
	class AArmorActor* ArmorActor;

	UPROPERTY()
	class AMedKitActor* MedKitActor;

	UPROPERTY()
	class AFirstAidKitActor* FirstAidKitActor;

	UPROPERTY()
	class AAdrenalineSyringe* AdrenalineSyringe;

	UPROPERTY()
	class APoisonOfSpider* PoisonOfSpider;

	UPROPERTY()
	class ARifleActor* rifleActor;

	UPROPERTY()
	class ASniperActor* sniperActor;

	UPROPERTY()
	class APistolActor* pistolActor;

	UPROPERTY()
	class AM249Actor* m249Actor;

	UPROPERTY()
	class ARifleMagActor* RifleMagActor;
	
	UPROPERTY()
	class ASniperMagActor* SniperMagActor;
	
	UPROPERTY()
	class APistolMagActor* PistolMagActor;
	
	UPROPERTY()
	class AM249MagActor* M249MagActor;

	UPROPERTY()
	class AMilitaryLaptop* MilitaryLaptop;

	UPROPERTY()
	class AMilitaryDevice* MilitaryDevice;

	UPROPERTY()
	class ARifleAmmoPack* RifleAmmoPack;

	UPROPERTY()
	class ASniperAmmoPack* SniperAmmoPack;

	UPROPERTY()
	class APistolAmmoPack* PistolAmmoPack;

	UPROPERTY()
	class AM249AmmoPack* M249AmmoPack;
	
	UPROPERTY()
	class AHackingConsole* HackingConsole;

	UPROPERTY()
	class AMissionChecker* MissionChecker;

	UPROPERTY()
	class AStageBoard* StageBoard;

	UPROPERTY()
	class AQuitGameActor* QuitGameActor;

	UPROPERTY()
	class ADeadPlayerContainer* DeadPlayerContainer;

	UPROPERTY()
	class AStash* Stash;

	UPROPERTY()
	class ATrader* Trader;
	
	UPROPERTY()
	bool TickOverlapBoolean = false;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	

		
};
