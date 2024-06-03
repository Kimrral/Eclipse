// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Boss.h"

#include "Components/CapsuleComponent.h"
#include "Eclipse/AI/EclipseBossAIController.h"
#include "Eclipse/Item/RewardManagerComponent.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/UI/BossShieldWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABoss::ABoss()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ABoss::SetAIController();

	ShieldWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ShieldWidgetComponent"));
	ShieldWidgetComponent->SetupAttachment(GetMesh());
	ShieldWidgetComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();

	EnemyStat->OnHpZero.AddUObject(this, &ABoss::OnDie);
}


void ABoss::OnDie()
{
	if (const auto AIController = Cast<AEclipseBossAIController>(GetController()); ::IsValid(AIController))
	{
		AIController->StopAI();
	}
	StopAnimMontage();
	PlayAnimMontage(AnimMontage, 1, FName("Death"));
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCharacterMovement()->Deactivate();
	GetWorld()->GetTimerManager().ClearTimer(StunHandle);
	UCapsuleComponent* const Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnDestroy();
}

void ABoss::OnDestroy()
{
	if (HasAuthority())
	{
		SetLifeSpan(30.f);
		RewardManager->DropRewardServer(GetActorTransform());
	}
}

void ABoss::OnShieldDestroy()
{
	if (::IsValid(EnemyStat))
	{
		ShieldDestroySuccessDelegate.ExecuteIfBound();

		EnemyStat->IsShieldBroken = true;
		EnemyStat->IsStunned = true;

		if (const auto AIController = Cast<AEclipseBossAIController>(GetController()); ::IsValid(AIController))
		{
			AIController->StopAI();
		}

		if (ShieldWidgetComponent->IsVisible())
		{
			ShieldWidgetComponent->SetVisibility(false);
		}

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShieldBreakSound, GetActorLocation(), FRotator::ZeroRotator);
		FTransform EmitterTrans = GetMesh()->GetSocketTransform(FName("ShieldSocket"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldBreakEmitter, EmitterTrans);
		EmitterTrans.SetScale3D(FVector(5));

		// 움직임 즉시 중단
		GetCharacterMovement()->StopMovementImmediately();
		// Movement Mode = None [움직임 차단]
		GetCharacterMovement()->SetMovementMode(MOVE_None);
		bUseControllerRotationYaw = false;
		StopAnimMontage();
		PlayAnimMontage(AnimMontage, 1, FName("StunStart"));
		GetWorld()->GetTimerManager().SetTimer(StunHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			EnemyStat->IsStunned = false;
			StopAnimMontage();
			// Movement Mode = Walking [움직임 재개]
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			bUseControllerRotationYaw = true;
			// Shield 회복
			EnemyStat->SetShield(EnemyStat->GetMaxShield());
			EnemyStat->IsShieldBroken = false;

			if (const auto AIController = Cast<AEclipseBossAIController>(GetController()); ::IsValid(AIController))
			{
				AIController->RunAI();
			}
		}), 10.0f, false);
	}
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

void ABoss::SetBossShieldWidget()
{
	EnemyStat->SetShield(EnemyStat->GetMaxShield());
	SetBossShieldWidgetServer();
}

void ABoss::SetBossShieldWidgetServer_Implementation()
{
	SetBossShieldWidgetMulticast();
}

void ABoss::SetBossShieldWidgetMulticast_Implementation()
{
	EnemyStat->OnShieldChanged.AddUObject(this, &ABoss::SetBossShieldWidgetDelegate);

	ShieldWidgetComponent->SetVisibility(true);
	if (BossShieldWidget = Cast<UBossShieldWidget>(ShieldWidgetComponent->GetUserWidgetObject()); ::IsValid(BossShieldWidget))
	{
		BossShieldWidget->UpdateShieldWidget(EnemyStat->GetCurrentShield(), EnemyStat->GetMaxShield());
		BossShieldWidget->PlayAnimation(BossShieldWidget->WidgetStart, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
	}
}

void ABoss::SetBossShieldWidgetDelegate(const float InCurShield, const float InMaxShield) const
{
	BossShieldWidget->UpdateShieldWidget(InCurShield, InMaxShield);
}

void ABoss::LaunchBossCharacter()
{
	LaunchCharacter(GetActorForwardVector() * DashForce, false, false);
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
}
