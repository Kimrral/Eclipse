// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Trigger/BossSpawnManager.h"

#include "Components/BoxComponent.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/Enemy/Boss.h"

// Sets default values
ABossSpawnManager::ABossSpawnManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	SpawnTriggerBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SpawnTriggerBoxCollision->SetGenerateOverlapEvents(true);
	SpawnTriggerBoxCollision->SetupAttachment(RootComponent);

	SpawnTriggerBoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABossSpawnManager::OnOverlap);
}


// Called when the game starts or when spawned
void ABossSpawnManager::BeginPlay()
{
	Super::BeginPlay();
}

void ABossSpawnManager::ActivateBossSpawnTimer()
{
	FTimerHandle EnemySpawnHandle;
	GetWorldTimerManager().SetTimer(EnemySpawnHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		IsSpawnable = true;
	}), SpawnCoolTime, false);
}

void ABossSpawnManager::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (const auto Player = Cast<APlayerCharacter>(OtherActor); Player && IsSpawnable)
		{
			if (Player->HasAuthority())
			{
				IsSpawnable = false;
				const FVector SpawnLoc = FVector(-5265, 14628, -532);
				const FRotator SpawnRot = FRotator(0, -50, 0);
				FActorSpawnParameters Param;
				Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnedBoss = GetWorld()->SpawnActor<ABoss>(BossFactory, SpawnLoc, SpawnRot, Param);
			}
			if (::IsValid(SpawnedBoss) && !SpawnedBoss->BossDeathDelegate.Contains(this, TEXT("ActivateBossSpawnTimer")))
			{
				SpawnedBoss->BossDeathDelegate.AddDynamic(this, &ABossSpawnManager::ActivateBossSpawnTimer);
			}
		}
	}
}
