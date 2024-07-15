// Fill out your copyright notice in the Description page of Project Settings.

#include "Eclipse/AI/EclipseBossAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "EclipseAI.h"
#include "Eclipse/Enemy/Boss.h"

AEclipseBossAIController::AEclipseBossAIController()
{
	// 블랙보드 데이터와 행동 트리 자산을 로드하여 초기화합니다.
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BlackBoardAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/KHJContent/AI/BB_Enemy.BB_Enemy'"));
	if (nullptr != BlackBoardAssetRef.Object)
	{
		// 블랙보드 데이터 자산을 ECBlackboard에 할당
		ECBlackboard = BlackBoardAssetRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/KHJContent/AI/BT_Enemy.BT_Enemy'"));
	if (nullptr != BehaviorTreeAssetRef.Object)
	{
		// 행동 트리 자산을 ECBehaviorTree에 할당
		ECBehaviorTree = BehaviorTreeAssetRef.Object;
	}
}

void AEclipseBossAIController::RunAI()
{
	// 블랙보드 컴포넌트를 가져와서 유효성 검사 후 블랙보드 데이터 사용
	if (UBlackboardComponent* BlackboardPtr = Blackboard.Get(); UseBlackboard(ECBlackboard, BlackboardPtr))
	{
		const FVector& InitialPos = FVector(-5265, 14628, -532); // 초기 위치를 설정 (주석 처리된 위치는 대체 위치)
		Blackboard->SetValueAsVector(BBKEY_INITIALPOS, InitialPos); // 블랙보드에 초기 위치 값 설정
		
		RunBehaviorTree(ECBehaviorTree); // 행동 트리를 실행
	}
}

void AEclipseBossAIController::StopAI() const
{
	// 현재 AI 컨트롤러의 브레인 컴포넌트를 UBehaviorTreeComponent로 캐스팅하고 행동 트리 중지
	if (UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		BehaviorTreeComponent->StopTree();
	}
}

void AEclipseBossAIController::BossInitialize() const
{
	// 현재 AI가 제어하는 폰을 ABoss로 캐스팅하고 유효성 검사 후 적의 상태 초기화
	if(const ABoss* const ControllingBoss = Cast<ABoss>(GetPawn()); ::IsValid(ControllingBoss))
	{
		ControllingBoss->InitializeStat(); // 보스의 상태 초기화
	}
}

void AEclipseBossAIController::OnPossess(APawn* InPawn)
{
	// AI가 폰을 제어하게 될 때 호출되는 함수
	Super::OnPossess(InPawn); // 부모 클래스의 OnPossess 호출
	
	RunAI(); // AI 실행
}

void AEclipseBossAIController::OnUnPossess()
{
	// AI가 폰의 제어를 해제할 때 호출되는 함수
	Super::OnUnPossess(); // 부모 클래스의 OnUnPossess 호출
	
	StopAI(); // AI 중지
}
