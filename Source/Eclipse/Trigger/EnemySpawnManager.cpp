// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnManager.h"

#include "Components/BoxComponent.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/Enemy/Enemy.h"

// Sets default values
AEnemySpawnManager::AEnemySpawnManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	SpawnTriggerBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SpawnTriggerBoxCollision->SetGenerateOverlapEvents(true);
	SpawnTriggerBoxCollision->SetupAttachment(RootComponent);		
	
	SpawnPosition = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPosition"));
	SpawnPosition->SetupAttachment(RootComponent);

	SpawnTriggerBoxCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnemySpawnManager::OnOverlap);
	SpawnTriggerBoxCollision->OnComponentEndOverlap.AddUniqueDynamic(this, &AEnemySpawnManager::EndOverlap);
}

// Called when the game starts or when spawned
void AEnemySpawnManager::BeginPlay()
{
	Super::BeginPlay();

	
}


void AEnemySpawnManager::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (const auto Player = Cast<APlayerCharacter>(OtherActor); Player && Player->HasAuthority() && IsSpawnable)
		{
			IsSpawnable = false;
			const FTransform SpawnTrans = SpawnPosition->GetComponentTransform();
			FActorSpawnParameters Param;
			Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			GetWorld()->SpawnActor<AEnemy>(EnemyFactory, SpawnTrans, Param);
			FTimerHandle EnemySpawnHandle;
			GetWorldTimerManager().SetTimer(EnemySpawnHandle, FTimerDelegate::CreateLambda([this]()-> void
			{
				IsSpawnable = true;
			}), SpawnCoolTime, false);
		}
	}
}

void AEnemySpawnManager::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
