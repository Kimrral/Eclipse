// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Animation/BossAnim.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

void UBossAnim::AnimNotify_MontageEnd() const
{
	MontageSectionFinishedDelegate.ExecuteIfBound();
}

void UBossAnim::AnimNotify_GroundSmashHitPoint() const
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
					if (!PlayerChar->GetMovementComponent()->IsFalling() && !PlayerChar->IsPlayerDeadImmediately)
					{
						PlayerChar->Damaged(50.f, GetOwningActor());
						return;
					}
				}
			}
		}
	}
}
