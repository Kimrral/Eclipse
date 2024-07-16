// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Animation/BossAnim.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/Enemy/Boss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

void UBossAnim::AnimNotify_MontageEnd() const
{
    // 애니메이션 몽타주가 종료되었음을 알리는 델리게이트를 실행합니다.
    MontageSectionFinishedDelegate.ExecuteIfBound();
}

void UBossAnim::AnimNotify_GroundSmashHitPoint() const
{
    // 지면 강타 공격 시 호출되는 함수로, GroundNotifyAttack 함수를 호출하여 50의 데미지를 적용합니다.
    GroundNotifyAttack(50.f);
}

void UBossAnim::AnimNotify_JumpAttackHitPoint() const
{
    // 점프 공격 시 호출되는 함수로, GroundNotifyAttack 함수를 호출하여 30의 데미지를 적용합니다.
    GroundNotifyAttack(30.f);
}

void UBossAnim::AnimNotify_BasicAttackHitPoint() const
{
    // 기본 공격 시 호출되는 함수로, 범위 내의 적에게 5의 데미지를 적용합니다.
    if (const AActor* OwnerActor = TryGetPawnOwner(); ::IsValid(OwnerActor) && OwnerActor->HasAuthority())
    {
        const FVector Center = OwnerActor->GetActorLocation();  // 공격 중심 위치를 현재 액터의 위치로 설정합니다.
        const UWorld* World = OwnerActor->GetWorld();  // 현재 월드를 가져옵니다.
        if (nullptr == World)
        {
            return;  // 월드가 유효하지 않으면 함수를 종료합니다.
        }

        TArray<FOverlapResult> OverlapResults;  // 오버랩 결과를 저장할 배열을 선언합니다.
        const FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, OwnerActor);  // 충돌 쿼리 파라미터를 설정합니다.

        // 원형 범위 내의 모든 액터와의 충돌을 검사합니다.
        if (bool bResult = World->OverlapMultiByChannel(
            OverlapResults,
            Center,
            FQuat::Identity,
            ECC_Pawn,
            FCollisionShape::MakeSphere(120.f),
            CollisionQueryParam
        ))
        {
            // 충돌 결과를 순회하며 플레이어 캐릭터에게 데미지를 적용합니다.
            for (auto const& OverlapResult : OverlapResults)
            {
                if (const auto PlayerChar = Cast<APlayerCharacter>(OverlapResult.GetActor()); ::IsValid(PlayerChar))
                {
                    if (!PlayerChar->IsPlayerDeadImmediately)  // 플레이어가 즉사 상태가 아니면
                    {
                        PlayerChar->Damaged(5.f, GetOwningActor());  // 플레이어에게 5의 데미지를 입힙니다.
                    }
                }
            }
        }
    }
}

void UBossAnim::AnimNotify_UltimateHitPoint() const
{
    // 궁극기 공격 시 호출되는 함수로, 범위 내의 적에게 큰 데미지를 적용하고, 보스의 방어막 위젯을 비활성화합니다.
    if (AActor* OwnerActor = TryGetPawnOwner(); ::IsValid(OwnerActor) && OwnerActor->HasAuthority())
    {
        const FVector Center = OwnerActor->GetActorLocation();  // 공격 중심 위치를 현재 액터의 위치로 설정합니다.
        const UWorld* World = OwnerActor->GetWorld();  // 현재 월드를 가져옵니다.
        if (nullptr == World)
        {
            return;  // 월드가 유효하지 않으면 함수를 종료합니다.
        }

        if (ABoss* OwnerBoss = Cast<ABoss>(OwnerActor); ::IsValid(OwnerBoss))
        {
            OwnerBoss->SetBossShieldWidget(false);  // 보스의 방어막 위젯을 비활성화합니다.
        }

        TArray<FOverlapResult> OverlapResults;  // 오버랩 결과를 저장할 배열을 선언합니다.
        const FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, OwnerActor);  // 충돌 쿼리 파라미터를 설정합니다.

        // 매우 넓은 범위 내의 모든 액터와의 충돌을 검사합니다.
        if (bool bResult = World->OverlapMultiByChannel(
            OverlapResults,
            Center,
            FQuat::Identity,
            ECC_Pawn,
            FCollisionShape::MakeSphere(5000.f),
            CollisionQueryParam
        ))
        {
            // 충돌 결과를 순회하며 플레이어 캐릭터에게 큰 데미지를 적용합니다.
            for (auto const& OverlapResult : OverlapResults)
            {
                if (const auto PlayerChar = Cast<APlayerCharacter>(OverlapResult.GetActor()); ::IsValid(PlayerChar))
                {
                    if (!PlayerChar->IsPlayerDeadImmediately && !PlayerChar->IsAlreadyDamaged)
                    {
                        PlayerChar->Damaged(200.f, GetOwningActor());  // 플레이어에게 200의 데미지를 입힙니다.
                        PlayerChar->IsAlreadyDamaged = true;  // 플레이어를 이미 데미지 입은 상태로 표시합니다.
                    }
                }
            }

            // 데미지를 입힌 플레이어의 IsAlreadyDamaged 상태를 리셋합니다.
            for (auto const& OverlapResult : OverlapResults)
            {
                if (const auto PlayerChar = Cast<APlayerCharacter>(OverlapResult.GetActor()); ::IsValid(PlayerChar))
                {
                    PlayerChar->IsAlreadyDamaged = false;
                }
            }
        }
    }
}

void UBossAnim::AnimNotify_DashEnd() const
{
    // 대시 공격 종료 시 호출되는 함수로, 보스 캐릭터의 이동 모드를 걷기로 설정하고, 워프 타겟을 제거합니다.
    if (const auto BossCharacter = Cast<ABoss>(TryGetPawnOwner()); ::IsValid(BossCharacter))
    {
        const FName& WarpTargetName = FName("SwiftTargetLocation");  // 워프 타겟의 이름을 설정합니다.
        BossCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);  // 이동 모드를 걷기로 변경합니다.
        BossCharacter->MotionWarpingComponent->RemoveWarpTarget(WarpTargetName);  // 워프 타겟을 제거합니다.
    }
}

void UBossAnim::GroundNotifyAttack(const float InDamage) const
{
    // 지면 공격 시 호출되는 함수로, 범위 내의 적에게 데미지를 적용합니다.
    if (const AActor* OwnerActor = TryGetPawnOwner(); ::IsValid(OwnerActor) && OwnerActor->HasAuthority())
    {
        const FVector Center = OwnerActor->GetActorLocation();  // 공격 중심 위치를 현재 액터의 위치로 설정합니다.
        const UWorld* World = OwnerActor->GetWorld();  // 현재 월드를 가져옵니다.
        if (nullptr == World)
        {
            return;  // 월드가 유효하지 않으면 함수를 종료합니다.
        }

        TArray<FOverlapResult> OverlapResults;  // 오버랩 결과를 저장할 배열을 선언합니다.
        const FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, OwnerActor);  // 충돌 쿼리 파라미터를 설정합니다.

        // 넓은 범위 내의 모든 액터와의 충돌을 검사합니다.
        if (bool bResult = World->OverlapMultiByChannel(
            OverlapResults,
            Center,
            FQuat::Identity,
            ECC_Pawn,
            FCollisionShape::MakeSphere(2000.f),
            CollisionQueryParam
        ))
        {
            // 충돌 결과를 순회하며 플레이어 캐릭터에게 데미지를 적용합니다.
            for (auto const& OverlapResult : OverlapResults)
            {
                if (const auto PlayerChar = Cast<APlayerCharacter>(OverlapResult.GetActor()); ::IsValid(PlayerChar))
                {
                    if (!PlayerChar->GetMovementComponent()->IsFalling() && !PlayerChar->IsPlayerDeadImmediately && !PlayerChar->IsAlreadyDamaged)
                    {
                        PlayerChar->Damaged(InDamage, GetOwningActor());  // 플레이어에게 전달된 데미지를 입힙니다.
                        PlayerChar->IsAlreadyDamaged = true;  // 플레이어를 이미 데미지 입은 상태로 표시합니다.
                    }
                }
            }

            // 데미지를 입힌 플레이어의 IsAlreadyDamaged 상태를 리셋합니다.
            for (auto const& OverlapResult : OverlapResults)
            {
                if (const auto PlayerChar = Cast<APlayerCharacter>(OverlapResult.GetActor()); ::IsValid(PlayerChar))
                {
                    PlayerChar->IsAlreadyDamaged = false;
                }
            }
        }
    }
}
