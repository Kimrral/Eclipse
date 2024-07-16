// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Artillery.h"

#include "GuardianProjectile.h"
#include "Eclipse/AI/EnemyFSM.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/Item/RewardManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AArtillery::AArtillery()
{
    // 적 FSM(상태 머신)을 생성합니다.
    EnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("EnemyFSM"));
    
    // 발사기(Launcher)의 스켈레탈 메쉬 컴포넌트를 생성합니다.
    LauncherComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LauncherComp"));
    
    // 발사기를 메쉬의 'hand_r' 소켓에 연결합니다.
    LauncherComp->SetupAttachment(GetMesh(), FName("hand_r"));
}

void AArtillery::BeginPlay()
{
    Super::BeginPlay();

    // 발사기 컴포넌트의 첫 번째 슬롯에 있는 머티리얼을 가져옵니다.
    const auto Material = LauncherComp->GetMaterial(0);
    
    // 동적 머티리얼 인스턴스를 생성하고, 발사기 컴포넌트의 머티리얼로 설정합니다.
    if (UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this))
    {
        LauncherComp->SetMaterial(0, DynamicMaterial);
    }
}

void AArtillery::OnDestroy()
{
    Super::OnDestroy();
    
    // 서버 권한이 있는 경우, M249 아이템을 떨어뜨립니다.
    if (HasAuthority())
    {        
        RewardManager->DropM249Server(GetActorTransform());
    }
}

void AArtillery::SetDissolveValue(const float Value)
{
    Super::SetDissolveValue(Value);
    
    // Dissolve 값을 0에서 1 사이로 보간합니다.
    const double Lerp = UKismetMathLibrary::Lerp(0, 1, Value);
    
    // 발사기 컴포넌트의 첫 번째 슬롯에 있는 동적 머티리얼 인스턴스를 가져옵니다.
    UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(LauncherComp->GetMaterial(0));
    
    // DissolveParams 매개변수를 설정하여 머티리얼의 dissolve 효과를 조정합니다.
    DynamicMaterial->SetScalarParameterValue("DissolveParams", Lerp);
}

void AArtillery::FireProcess() const
{
    // 플레이어가 존재하는 경우, 발사 과정을 처리합니다.
    if (EnemyFSM->Player)
    {
        // 발사기(ArtilleryMuzzle) 소켓의 변환을 가져옵니다.
        const FTransform MuzzleTrans = GetMesh()->GetSocketTransform(FName("ArtilleryMuzzle"));
        
        // 발사기와 플레이어 사이의 방향 벡터를 계산합니다.
        const FVector PlayerLoc = (EnemyFSM->Player->GetActorLocation() - MuzzleTrans.GetLocation());
        
        // 발사기 방향과 플레이어의 위치를 바탕으로 발사체의 회전값을 계산합니다.
        const FRotator ProjectileRot = UKismetMathLibrary::MakeRotFromXZ(PlayerLoc, this->GetActorUpVector());
        
        // 발사체를 생성합니다.
        GetWorld()->SpawnActor<AGuardianProjectile>(GuardianProjectileFactory, MuzzleTrans.GetLocation(), ProjectileRot);
    }
}
