// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Animation/BossAnim.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/Enemy/Boss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

void UBossAnim::AnimNotify_MontageEnd() const
{
	MontageSectionFinishedDelegate.ExecuteIfBound();
}

void UBossAnim::AnimNotify_GroundSmashHitPoint() const
{
	GroundNotifyAttack(50.f);
}

void UBossAnim::AnimNotify_JumpAttackHitPoint() const
{
	GroundNotifyAttack(30.f);
}

void UBossAnim::AnimNotify_BasicAttackHitPoint() const
{
	if (const AActor* OwnerActor = TryGetPawnOwner(); ::IsValid(OwnerActor) && OwnerActor->HasAuthority())
	{
		const FVector Center = OwnerActor->GetActorLocation();
		const UWorld* World = OwnerActor->GetWorld();
		if (nullptr == World)
		{
			return;
		}

		TArray<FOverlapResult> OverlapResults;
		const FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, OwnerActor);

		if (bool bResult = World->OverlapMultiByChannel(
			OverlapResults,
			Center,
			FQuat::Identity,
			ECC_Pawn,
			FCollisionShape::MakeSphere(120.f),
			CollisionQueryParam
		))
		{
			for (auto const& OverlapResult : OverlapResults)
			{
				if (const auto PlayerChar = Cast<APlayerCharacter>(OverlapResult.GetActor()); ::IsValid(PlayerChar))
				{
					if (!PlayerChar->IsPlayerDeadImmediately)
					{
						PlayerChar->Damaged(5.f, GetOwningActor());
					}
				}
			}
		}
	}
}

void UBossAnim::AnimNotify_UltimateHitPoint() const
{
	if (AActor* OwnerActor = TryGetPawnOwner(); ::IsValid(OwnerActor) && OwnerActor->HasAuthority())
	{
		const FVector Center = OwnerActor->GetActorLocation();
		const UWorld* World = OwnerActor->GetWorld();
		if (nullptr == World)
		{
			return;
		}

		if (ABoss* OwnerBoss = Cast<ABoss>(OwnerActor); ::IsValid(OwnerBoss))
		{
			OwnerBoss->SetBossShieldWidget(false);
		}

		TArray<FOverlapResult> OverlapResults;
		const FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, OwnerActor);

		if (bool bResult = World->OverlapMultiByChannel(
			OverlapResults,
			Center,
			FQuat::Identity,
			ECC_Pawn,
			FCollisionShape::MakeSphere(5000.f),
			CollisionQueryParam
		))
		{
			for (auto const& OverlapResult : OverlapResults)
			{
				if (const auto PlayerChar = Cast<APlayerCharacter>(OverlapResult.GetActor()); ::IsValid(PlayerChar))
				{
					if (!PlayerChar->IsPlayerDeadImmediately && !PlayerChar->IsAlreadyDamaged)
					{
						PlayerChar->Damaged(200.f, GetOwningActor());
						PlayerChar->IsAlreadyDamaged = true;
					}
				}
			}
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
	if (const auto BossCharacter = Cast<ABoss>(TryGetPawnOwner()); ::IsValid(BossCharacter))
	{
		const FName& WarpTargetName = FName("SwiftTargetLocation");
		BossCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		BossCharacter->MotionWarpingComponent->RemoveWarpTarget(WarpTargetName);
	}
}


void UBossAnim::GroundNotifyAttack(const float InDamage) const
{
	if (const AActor* OwnerActor = TryGetPawnOwner(); ::IsValid(OwnerActor) && OwnerActor->HasAuthority())
	{
		const FVector Center = OwnerActor->GetActorLocation();
		const UWorld* World = OwnerActor->GetWorld();
		if (nullptr == World)
		{
			return;
		}

		TArray<FOverlapResult> OverlapResults;
		const FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, OwnerActor);

		if (bool bResult = World->OverlapMultiByChannel(
			OverlapResults,
			Center,
			FQuat::Identity,
			ECC_Pawn,
			FCollisionShape::MakeSphere(2000.f),
			CollisionQueryParam
		))
		{
			for (auto const& OverlapResult : OverlapResults)
			{
				if (const auto PlayerChar = Cast<APlayerCharacter>(OverlapResult.GetActor()); ::IsValid(PlayerChar))
				{
					if (!PlayerChar->GetMovementComponent()->IsFalling() && !PlayerChar->IsPlayerDeadImmediately && !PlayerChar->IsAlreadyDamaged)
					{
						PlayerChar->Damaged(InDamage, GetOwningActor());
						PlayerChar->IsAlreadyDamaged = true;
					}
				}
			}
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
