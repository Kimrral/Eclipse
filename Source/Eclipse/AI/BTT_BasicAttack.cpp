// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/BTT_BasicAttack.h"

#include "AIController.h"
#include "Eclipse/Animation/BossAnim.h"
#include "Eclipse/Enemy/Boss.h"

// UBTT_BasicAttack 클래스의 생성자
UBTT_BasicAttack::UBTT_BasicAttack()
{
}

// 비헤이비어 트리 태스크를 실행하는 함수
EBTNodeResult::Type UBTT_BasicAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 부모 클래스의 ExecuteTask 함수를 호출하고 그 결과를 저장
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI 컨트롤러가 제어하는 폰을 가져옴
	if (APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn(); ::IsValid(ControllingPawn))
	{
		// 제어하는 폰을 ABoss 타입으로 캐스팅
		if (ABoss* ControllingBoss = Cast<ABoss>(ControllingPawn))
		{
			// 캐스팅이 성공하고 보스가 유효하면
			if (::IsValid(ControllingBoss))
			{
				// 0에서 3 사이의 랜덤 인덱스를 생성
				if (const auto RandIndex = FMath::RandRange(0, 3); RandIndex >= 0 && RandIndex <= 3)
				{
					// 랜덤 인덱스에 따라 공격 애니메이션 섹션 이름을 생성
					const FString SectionName = FString::Printf(TEXT("PrimaryAttack%d"), RandIndex + 1);
					// 보스가 해당 섹션 이름의 애니메이션 몽타주를 재생
					ControllingBoss->PlayAnimMontageBySectionName(FName(*SectionName));
				}
				
				// 보스의 애니메이션 인스턴스를 가져옴
				if (const auto BossAnimInstance = Cast<UBossAnim>(ControllingBoss->GetMesh()->GetAnimInstance()); ::IsValid(BossAnimInstance))
				{
					// 애니메이션 몽타주 섹션이 끝났을 때 호출될 람다 함수 바인딩
					BossAnimInstance->MontageSectionFinishedDelegate.BindLambda(
						[&]()
						{
							// 태스크가 성공적으로 완료되었음을 알림
							FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
						}
					);
				}
			}
		}
	}
	else
	{
		// 제어하는 폰이 유효하지 않으면 태스크 실패를 반환
		return EBTNodeResult::Failed;
	}

	// 태스크가 진행 중임을 반환
	return EBTNodeResult::InProgress;
}

