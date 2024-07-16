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

// 기본 값을 설정합니다.
ABoss::ABoss()
{
    // 이 캐릭터가 매 프레임마다 Tick()을 호출하도록 설정합니다. 필요하지 않을 경우 이 기능을 끄면 성능이 향상될 수 있습니다.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    // AI 컨트롤러를 설정합니다.
    ABoss::SetAIController();

    // 실드 위젯 컴포넌트를 생성하고, 메쉬에 부착합니다.
    ShieldWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ShieldWidgetComponent"));
    ShieldWidgetComponent->SetupAttachment(GetMesh());
    ShieldWidgetComponent->SetVisibility(false);

    // 모션 워핑 컴포넌트를 생성합니다.
    MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

// 게임이 시작되거나 스폰될 때 호출됩니다.
void ABoss::BeginPlay()
{
    Super::BeginPlay();
}

// 보스가 죽었을 때 호출됩니다.
void ABoss::OnDie()
{
    // AI 컨트롤러가 유효하다면 AI를 멈춥니다.
    if (const auto AIController = Cast<AEclipseBossAIController>(GetController()); ::IsValid(AIController))
    {
        AIController->StopAI();
    }
    
    // 현재 재생 중인 애니메이션 몽타주를 중지하고, 'Death' 애니메이션 몽타주를 재생합니다.
    StopAnimMontage();
    PlayAnimMontage(AnimMontage, 1, FName("Death"));
    
    // 캐릭터의 이동 모드를 정지 상태로 설정하고, 충돌을 비활성화합니다.
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->SetMovementMode(MOVE_None);
    GetCharacterMovement()->Deactivate();
    
    // 스턴 핸들러 타이머를 지웁니다.
    GetWorld()->GetTimerManager().ClearTimer(StunHandle);
    
    // 캡슐 컴포넌트와 메쉬의 충돌을 비활성화합니다.
    UCapsuleComponent* const Capsule = GetCapsuleComponent();
    Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // 보스 죽음 델리게이트를 호출합니다.
    BossDeathDelegate.Broadcast();
    OnDestroy();
}

// 보스가 파괴될 때 호출됩니다.
void ABoss::OnDestroy()
{
    // 권한이 있는 경우, 보스의 생명 시간을 30초로 설정하고 보상 아이템을 떨어뜨립니다.
    if (HasAuthority())
    {
        SetLifeSpan(30.f);
        RewardManager->DropRewardServer(GetActorTransform());
    }
}

// 실드가 파괴되었을 때 호출됩니다.
void ABoss::OnShieldDestroy()
{
    // 적의 상태가 유효한 경우, 실드 파괴 성공 델리게이트를 호출합니다.
    if (::IsValid(EnemyStat))
    {
        ShieldDestroySuccessDelegate.ExecuteIfBound();
        
        // 실드가 파괴되었음을 상태에 반영하고, 스턴 상태로 설정합니다.
        EnemyStat->IsShieldBroken = true;
        EnemyStat->IsStunned = true;

        // AI 컨트롤러가 유효하다면 AI를 멈춥니다.
        if (const auto AIController = Cast<AEclipseBossAIController>(GetController()); ::IsValid(AIController))
        {
            AIController->StopAI();
        }

        // 실드 위젯이 보이면 비활성화합니다.
        if (ShieldWidgetComponent->IsVisible())
        {
            ShieldWidgetComponent->SetVisibility(false);
        }

        // 실드 파괴 사운드를 재생합니다.
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShieldBreakSound, GetActorLocation(), FRotator::ZeroRotator);
        
        // 실드 파괴 이펙트를 재생합니다.
        FTransform EmitterTrans = GetMesh()->GetSocketTransform(FName("ShieldSocket"));
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldBreakEmitter, EmitterTrans);
        EmitterTrans.SetScale3D(FVector(5));

        // 즉시 움직임을 중단하고, 이동 모드를 None으로 설정합니다.
        GetCharacterMovement()->StopMovementImmediately();
        GetCharacterMovement()->SetMovementMode(MOVE_None);
        bUseControllerRotationYaw = false;
        
        // 애니메이션 몽타주를 중지하고, 스턴 시작 애니메이션 몽타주를 재생합니다.
        StopAnimMontage();
        PlayAnimMontage(AnimMontage, 1, FName("StunStart"));
        
        // 일정 시간 후에 스턴을 해제하고, 이동 모드를 Walking으로 되돌립니다.
        GetWorld()->GetTimerManager().SetTimer(StunHandle, FTimerDelegate::CreateLambda([this]()-> void
        {
            EnemyStat->IsStunned = false;
            StopAnimMontage();
            GetCharacterMovement()->SetMovementMode(MOVE_Walking);
            bUseControllerRotationYaw = true;
            
            // 실드를 회복시키고, 실드 파괴 상태를 해제합니다.
            EnemyStat->SetShield(EnemyStat->GetMaxShield());
            EnemyStat->IsShieldBroken = false;

            if (const auto AIController = Cast<AEclipseBossAIController>(GetController()); ::IsValid(AIController))
            {
                AIController->RunAI();
            }
        }), 8.0f, false);
    }
}

// AI 컨트롤러를 설정합니다.
void ABoss::SetAIController()
{
    // AI 컨트롤러 클래스를 설정하고, 자동으로 AI를 소유하도록 설정합니다.
    AIControllerClass = AEclipseBossAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// 실드 관련 머티리얼을 설정합니다. (현재는 빈 함수)
void ABoss::SetDissolveMaterial()
{
    return;
}

// 실드 관련 값을 설정합니다. (현재는 빈 함수)
void ABoss::SetDissolveValue(float Value)
{
    return;
}

// 상태를 초기화합니다.
void ABoss::InitializeStat() const
{
    InitializeStatServer();
}

// 서버에서 상태를 초기화합니다.
void ABoss::InitializeStatServer_Implementation() const
{
    // 최대 체력과 현재 체력이 일치하지 않는 경우, 체력과 실드를 최대 값으로 설정합니다.
    if (EnemyStat->GetMaxHp() == EnemyStat->GetCurrentHp() && EnemyStat->GetMaxShield() == EnemyStat->GetCurrentShield())
    {
        return;
    }
    EnemyStat->SetHp(EnemyStat->GetMaxHp());
    EnemyStat->SetShield(EnemyStat->GetMaxShield());
    UE_LOG(LogTemp, Warning, TEXT("Initialize"))
}

// 보스 실드 위젯을 설정합니다.
void ABoss::SetBossShieldWidget(const bool bEnable)
{
    SetBossShieldWidgetServer(bEnable);
}

// 서버에서 보스 실드 위젯을 설정합니다.
void ABoss::SetBossShieldWidgetServer_Implementation(const bool bEnable)
{
    if (bEnable)
    {
        // 실드를 최대값으로 설정합니다.
        EnemyStat->SetShield(EnemyStat->GetMaxShield());
    }
    SetBossShieldWidgetMulticast(bEnable);
}

// 멀티캐스트로 보스 실드 위젯을 설정합니다.
void ABoss::SetBossShieldWidgetMulticast_Implementation(const bool bEnable)
{
    // 보스 실드 위젯을 활성화합니다.
    if (bEnable)
    {
        if(!HasAuthority())
        {
            // 실드 변동 사항을 받을 이유가 없으므로, 동적으로 델리게이트를 바인딩합니다.
            if (!EnemyStat->OnShieldChanged.Contains(this, TEXT("SetBossShieldWidgetDelegate")))
            {
                EnemyStat->OnShieldChanged.AddDynamic(this, &ABoss::SetBossShieldWidgetDelegate);
            }
            // 위젯 컴포넌트의 가시성을 설정합니다.
            ShieldWidgetComponent->SetVisibility(true);
        }        

        // 위젯을 보여주기 전에 현재 실드 값을 업데이트합니다.
        if (BossShieldWidget = Cast<UBossShieldWidget>(ShieldWidgetComponent->GetUserWidgetObject()); ::IsValid(BossShieldWidget))
        {
            BossShieldWidget->UpdateShieldWidget(EnemyStat->GetCurrentShield(), EnemyStat->GetMaxShield());
            BossShieldWidget->PlayAnimation(BossShieldWidget->WidgetStart, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
        }
    }
    // 보스 실드 위젯을 비활성화합니다.
    else
    {
        if(!HasAuthority())
        {
            if (EnemyStat->OnShieldChanged.Contains(this, TEXT("SetBossShieldWidgetDelegate")))
            {
                // 델리게이트를 해제합니다.
                EnemyStat->OnShieldChanged.RemoveDynamic(this, &ABoss::SetBossShieldWidgetDelegate);
            }
            // 위젯 컴포넌트의 가시성을 설정합니다.
            ShieldWidgetComponent->SetVisibility(false);
        }
    }
}

// 보스 실드 위젯 델리게이트를 설정합니다.
void ABoss::SetBossShieldWidgetDelegate(const float InCurShield, const float InMaxShield)
{
    if (::IsValid(BossShieldWidget))
    {
        BossShieldWidget->UpdateShieldWidget(InCurShield, InMaxShield);
    }
}

// 보스 캐릭터를 발사하여 목표 위치로 이동시킵니다.
void ABoss::LaunchBossCharacter(const FVector& TargetLocation) const
{
    // 이동 모드를 비행으로 설정합니다.
    GetCharacterMovement()->SetMovementMode(MOVE_Flying);
    const FName& WarpTargetName = FName("SwiftTargetLocation");
    
    // 목표 위치와 현재 위치를 기준으로 최종 위치를 계산합니다.
    const FVector FinalLocation = TargetLocation + (GetActorLocation() - TargetLocation) * 150.f;
    
    // 모션 워핑 컴포넌트에 새로운 워프 타겟을 추가합니다.
    MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(WarpTargetName, FinalLocation);
}

// 애니메이션 몽타주를 특정 섹션 이름으로 재생합니다.
void ABoss::PlayAnimMontageBySectionName(const FName& SectionName)
{
    PlayAnimMontageBySectionNameServer(SectionName);
}

// 서버에서 애니메이션 몽타주를 특정 섹션 이름으로 재생합니다.
void ABoss::PlayAnimMontageBySectionNameServer_Implementation(const FName& SectionName)
{
    PlayAnimMontageBySectionNameMulticast(SectionName);
}

// 멀티캐스트로 애니메이션 몽타주를 특정 섹션 이름으로 재생합니다.
void ABoss::PlayAnimMontageBySectionNameMulticast_Implementation(const FName& SectionName)
{
    StopAnimMontage();
    PlayAnimMontage(AnimMontage, 1, SectionName);
}
