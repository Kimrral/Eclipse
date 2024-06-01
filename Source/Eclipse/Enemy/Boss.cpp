// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Boss.h"

#include "Components/CapsuleComponent.h"
#include "Eclipse/AI/EclipseBossAIController.h"
#include "Eclipse/Item/RewardManagerComponent.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABoss::ABoss()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ABoss::SetAIController();
}

void ABoss::OnDie()
{
	if(const auto AIController = Cast<AEclipseBossAIController>(GetController()); ::IsValid(AIController))
	{
		AIController->StopAI();
	}
	StopAnimMontage();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCharacterMovement()->Deactivate();
	GetWorld()->GetTimerManager().ClearTimer(StunHandle);
	UCapsuleComponent* const Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnDestroy();

	PlayAnimMontage(AnimMontage, 1, FName("Death"));
}

void ABoss::OnDestroy()
{
	if (HasAuthority())
	{
		SetLifeSpan(30.f);
		RewardManager->DropRewardServer(GetActorTransform());
	}
}


// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();

	EnemyStat->OnHpZero.AddUObject(this, &ABoss::OnDie);
}


void ABoss::SetAIController()
{
	// AI Controller
	AIControllerClass = AEclipseBossAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


void ABoss::SetDissolveMaterial()
{
	return;
}

void ABoss::SetDissolveValue(float Value)
{
	return;
}

void ABoss::LaunchBossCharacter()
{
	LaunchCharacter(GetActorForwardVector()*DashForce, false, false);
}

void ABoss::PlayAnimMontageBySectionName(const FName& SectionName)
{
	PlayAnimMontageBySectionNameServer(SectionName);
}

void ABoss::PlayAnimMontageBySectionNameServer_Implementation(const FName& SectionName)
{
	PlayAnimMontageBySectionNameMulticast(SectionName);
}

void ABoss::PlayAnimMontageBySectionNameMulticast_Implementation(const FName& SectionName)
{
	StopAnimMontage();
	PlayAnimMontage(AnimMontage, 1, SectionName);
	UE_LOG(LogTemp, Warning, TEXT("Play Montage"))
}
