// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/AI/EclipseAIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


void AEclipseAIController::OnPossess(APawn* InPawn)
{
	// 빙의 시
	Super::OnPossess(InPawn);
}

void AEclipseAIController::OnUnPossess()
{
	// 빙의 해제 시
	Super::OnUnPossess();
}

void AEclipseAIController::RandomMove()
{
	// auto CurrentPawn = GetPawn();
	//
	// // 월드의 내비게이션 시스템을 가져온다
	// const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	//
	// if (NavSystem == nullptr)
	// 	return;
	//
	// FNavLocation RandomLocation;
	//
	// // 월드의 내비게이션 범위중 원점(첫번째 매개변수)을 기준으로 반지름 만큼의 범위를 한정지어
	// // 랜덤한 좌표를 가져온다(RandomLocation에 저장됨)
	// if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.f, RandomLocation))
	// {
	// 	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomLocation);
	// }
}

void AEclipseAIController::MoveToPlayer(const AActor* TargetPlayer)
{
	UAIBlueprintHelperLibrary::SimpleMoveToActor(this, TargetPlayer);
}

void AEclipseAIController::MoveToLocation(const FVector TargetLocation)
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TargetLocation);
}
