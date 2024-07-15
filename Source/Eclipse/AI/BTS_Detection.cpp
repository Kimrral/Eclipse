// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTS_Detection.h"

#include "AIController.h"
#include "EclipseAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Character/PlayerCharacter.h"

// UBTS_Detection 클래스의 생성자
UBTS_Detection::UBTS_Detection()
{
	// 이 비헤이비어 트리 노드의 이름을 설정
	NodeName = TEXT("Detection");
	// 노드의 실행 간격을 0.5초로 설정
	Interval = 0.5f;
}

// 비헤이비어 트리 노드의 틱(Tick) 함수
void UBTS_Detection::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, const float DeltaSeconds)
{
	// 부모 클래스의 TickNode 함수를 호출
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// AI 컨트롤러가 제어하는 폰을 가져옴
	const APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	// 제어하는 폰이 없으면 함수를 종료
	if (nullptr == ControllingPawn)
	{
		return;
	}

	// 폰의 현재 위치를 가져옴
	const FVector Center = ControllingPawn->GetActorLocation();
	// 폰이 속한 월드를 가져옴
	const UWorld* World = ControllingPawn->GetWorld();
	// 월드가 없으면 함수를 종료
	if (nullptr == World)
	{
		return;
	}

	// 충돌 쿼리 매개변수를 설정 (폴리곤 파악에 사용)
	const FCollisionQueryParams CollisionQueryParam(SCENE_QUERY_STAT(Detect), false, ControllingPawn);

	// 주어진 범위 내의 모든 폰을 감지하기 위한 충돌 검사
	if (TArray<FOverlapResult> OverlapResults; World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(DetectionRange),
		CollisionQueryParam
	))
	{
		// 감지된 플레이어 캐릭터들을 저장할 배열
		TArray<APlayerCharacter*> PlayerCharacterArray;
		// 가장 많은 데미지를 준 플레이어의 인덱스
		int MaxDamageIndex = 0;
		// 충돌 결과를 순회하면서 플레이어 캐릭터를 검사
		for (auto const& OverlapResult : OverlapResults)
		{
			// APlayerCharacter 타입으로 캐스팅하고 유효성을 검사
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(OverlapResult.GetActor()); ::IsValid(Player))
			{
				// 플레이어가 즉사 상태인지 확인
				if (Player->IsPlayerDeadImmediately)
				{
					continue;
				}
				// 플레이어가 보스에게 누적 데미지를 입혔는지 확인
				if (Player->Stat->AccumulatedDamageToBoss > 0)
				{
					// 누적 데미지가 있는 플레이어를 배열에 추가
					PlayerCharacterArray.Add(Player);
				}
			}
		}
		// 누적 데미지가 가장 많은 플레이어를 찾기 위한 반복문
		for (int i = 0; i < PlayerCharacterArray.Num(); i++)
		{
			// [MaxDamageIndex]번째 플레이어의 누적 데미지
			const float MaxDamage = PlayerCharacterArray[MaxDamageIndex]->Stat->AccumulatedDamageToBoss;
			// [i]번째 플레이어의 누적 데미지
			const float NextDamage = PlayerCharacterArray[i]->Stat->AccumulatedDamageToBoss;

			// 만약 이번 대상이 현재 대상보다 누적 데미지가 많다면
			if (NextDamage > MaxDamage)
			{
				// 누적 데미지가 많은 대상 플레이어로 변경하기
				MaxDamageIndex = i;
			}
		}
		// 인덱스가 유효한지 검사한 후 블랙보드에 목표 설정
		if (PlayerCharacterArray.IsValidIndex(MaxDamageIndex))
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, PlayerCharacterArray[MaxDamageIndex]);
			return;
		}
	}

	// 감지된 플레이어가 없으면 블랙보드에 목표를 null로 설정
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(BBKEY_TARGET, nullptr);
}
