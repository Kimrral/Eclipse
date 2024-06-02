// Fill out your copyright notice in the Description page of Project Settings.


#include "CrunchAnim.h"

#include "Eclipse/Enemy/Enemy.h"
#include "Eclipse/Character/PlayerCharacter.h"


void UCrunchAnim::AnimNotify_HitPoint() const
{
	if(me->HasAuthority())
	{
		// 중심점
		const FVector Center = me->GetActorLocation() + me->GetActorForwardVector() * 150.f;
		// 충돌체크(구충돌)
		// 충돌한 물체를 기억할 배열
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(me);
		// Overlap Multi
		if (TArray<FOverlapResult> HitObj; GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(80), Params))
		{
			// 충돌 배열 순회
			for (int i = 0; i < HitObj.Num(); ++i)
			{
				if (APlayerCharacter* Player = Cast<APlayerCharacter>(HitObj[i].GetActor()); Player&&Player->HasAuthority()&&Player->IsPlayerDeadImmediately==false)
				{
					Player->Damaged(50, GetOwningActor());
					return;
				}
			}
		}
	}
}
