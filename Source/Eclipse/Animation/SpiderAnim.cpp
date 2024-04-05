// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Animation/SpiderAnim.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/Enemy/Enemy.h"

void USpiderAnim::AnimNotify_HitPoint()
{
	if(me->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack"))
		// 중심점
		const FVector Center = me->GetActorLocation() + me->GetActorForwardVector() * 30.f;
		// 충돌체크(구충돌)
		// 충돌한 물체를 기억할 배열
		TArray<FOverlapResult> HitObj;;
		FCollisionQueryParams params;
		params.AddIgnoredActor(me);
		// Overlap Multi
		bool bOverlapHit = GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(50), params);
		if (bOverlapHit)
		{
			// 충돌 배열 순회
			for (int i = 0; i < HitObj.Num(); ++i)
			{
				APlayerCharacter* player = Cast<APlayerCharacter>(HitObj[i].GetActor());
				if (player&&player->HasAuthority()&&player->IsPlayerDeadImmediately==false)
				{
					player->Damaged(10, GetOwningActor()); 
				}
			}
		}
	}
}
