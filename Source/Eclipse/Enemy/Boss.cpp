// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Boss.h"

#include "Components/CapsuleComponent.h"
#include "Eclipse/AI/EclipseBossAIController.h"
#include "MotionWarpingComponent.h"
#include "Eclipse/Item/RewardManagerComponent.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/UI/BossShieldWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
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

void ABoss::SwiftAttack()
{
}

void ABoss::SetBossShieldWidget(const bool bEnable)
{
	SetBossShieldWidgetServer(bEnable);
}

void ABoss::SetBossShieldWidgetServer_Implementation(const bool bEnable)
{
	if (bEnable) EnemyStat->SetShield(EnemyStat->GetMaxShield());
	SetBossShieldWidgetMulticast(bEnable);
}

void ABoss::SetBossShieldWidgetMulticast_Implementation(const bool bEnable)
{
	// 보스 실드 위젯 활성화
	if (bEnable)
	{
		// 전멸기 차징 외에는 실드 변동사항을 전달받을 이유가 없으므로, 동적으로 델리게이트 바인딩과 해제
		EnemyStat->OnShieldChanged.AddUObject(this, &ABoss::SetBossShieldWidgetDelegate);

		// 위젯 컴포넌트 Visibility 설정
		ShieldWidgetComponent->SetVisibility(true);

		// 위젯을 보여주기 전에, 현재 실드 값 업데이트
		if (BossShieldWidget = Cast<UBossShieldWidget>(ShieldWidgetComponent->GetUserWidgetObject()); ::IsValid(BossShieldWidget))
		{
			BossShieldWidget->UpdateShieldWidget(EnemyStat->GetCurrentShield(), EnemyStat->GetMaxShield());
			BossShieldWidget->PlayAnimation(BossShieldWidget->WidgetStart, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
		}
	}
	// 보스 실드 위젯 비활성화
	else
	{
		// 델리게이트 해제
		EnemyStat->OnShieldChanged.Clear();
		// 위젯 컴포넌트 Visibility 설정
		ShieldWidgetComponent->SetVisibility(false);
	}
}

void ABoss::SetBossShieldWidgetDelegate(const float InCurShield, const float InMaxShield) const
{
	BossShieldWidget->UpdateShieldWidget(InCurShield, InMaxShield);
}

void ABoss::LaunchBossCharacter(const FVector& TargetLocation) const
{
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	const FName& WarpTargetName = FName("SwiftTargetLocation");
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(WarpTargetName, TargetLocation);
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
