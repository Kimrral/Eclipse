// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTS_Detection.h"

#include "AIController.h"
#include "EclipseAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Character/PlayerCharacter.h"

UBTS_Detection::UBTS_Detection()
{
	NodeName = TEXT("Detection");
	Interval = 0.5f;
}

void UBTS_Detection::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
	{
		return;
	}

	const FVector Center = ControllingPawn->GetActorLocation();
	const UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World)
	{
		return;
	}

	TArray<APlayerCharacter*> PlayerCharacterArray;

	// 함수 호출마다 캐릭터 배열 리셋
	PlayerCharacterArray.Reset();

	const FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, ControllingPawn);

	if (TArray<FOverlapResult> OverlapResults; World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(DetectionRange),
		CollisionQueryParam
	))
	{
		int MaxDamageIndex = 0;
		for (auto const& OverlapResult : OverlapResults)
		{
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(OverlapResult.GetActor()); ::IsValid(Player))
			{
				if(Player->IsPlayerDeadImmediately)
				{
					continue;
				}
				if(Player->Stat->AccumulatedDamageToBoss>0)
				{
					PlayerCharacterArray.Add(Player);
				}
			}
		}
		for (int i = 0; i < PlayerCharacterArray.Num(); i++)
		{
			// [MaxDistIndex]번째 플레이어 누적 데미지
			const float MaxDamage = PlayerCharacterArray[MaxDamageIndex]->Stat->AccumulatedDamageToBoss;
			// [i]번째 플레이어 누적 데미지
			const float NextDamage = PlayerCharacterArray[i]->Stat->AccumulatedDamageToBoss;

			// 만약 이번 대상이 현재 대상보다 축적 데미지가 많다면
			if (NextDamage > MaxDamage)
			{
				// 누적 데미지가 많은 대상 플레이어로 변경하기
				MaxDamageIndex = i;
			}
		}
		// 인덱스 유효 여부 검사 후 리턴
		if(PlayerCharacterArray.IsValidIndex(MaxDamageIndex))
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, PlayerCharacterArray[MaxDamageIndex]);
			return;
		}		
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, nullptr);
}
