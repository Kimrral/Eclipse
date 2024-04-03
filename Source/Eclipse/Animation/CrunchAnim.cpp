// Fill out your copyright notice in the Description page of Project Settings.


#include "CrunchAnim.h"

#include "Eclipse/Enemy/Enemy.h"
#include "Eclipse/Character/PlayerCharacter.h"


void UCrunchAnim::AnimNotify_HitPoint()
{
	if(me->HasAuthority())
	{
		// 중심점
		const FVector Center = me->GetActorLocation() + me->GetActorForwardVector() * 150.f;
		// 충돌체크(구충돌)
		// 충돌한 물체를 기억할 배열
		TArray<FOverlapResult> HitObj;;
		FCollisionQueryParams params;
		params.AddIgnoredActor(me);
		// Overlap Multi
		bool bOverlapHit = GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(80), params);
		if (bOverlapHit)
		{
			// 충돌 배열 순회
			for (int i = 0; i < HitObj.Num(); ++i)
			{
				APlayerCharacter* player = Cast<APlayerCharacter>(HitObj[i].GetActor());
				if (player&&player->HasAuthority()&&player->IsPlayerDeadImmediately==false)
				{
					player->Damaged(50, GetOwningActor()); 
				}
			}
		}
	}
}
