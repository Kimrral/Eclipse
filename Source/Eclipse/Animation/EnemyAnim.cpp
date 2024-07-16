// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"

#include "Eclipse/Enemy/Enemy.h"
#include "Eclipse/AI/EnemyFSM.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemyAnim::NativeBeginPlay()
{
	// 부모 클래스의 NativeBeginPlay를 호출하여 기본적인 초기화를 수행합니다.
	Super::NativeBeginPlay();

	// 현재 애니메이션의 소유 액터를 AEnemy 타입으로 캐스팅합니다.
	me = Cast<AEnemy>(TryGetPawnOwner());

	// AEnemy 액터의 "enemyFSM" 서브 객체를 UEnemyFSM 타입으로 캐스팅하여 FSM 변수에 할당합니다.
	FSM = Cast<UEnemyFSM>(me->GetDefaultSubobjectByName(FName("enemyFSM")));

	// FSM의 OnStateChanged 델리게이트에 ReplicateChangedState 함수를 바인딩합니다.
	FSM->OnStateChanged.AddUObject(this, &UEnemyAnim::ReplicateChangedState);
}

void UEnemyAnim::AnimNotify_AttackStart()
{
	// 공격 애니메이션이 시작될 때 호출되는 함수입니다.
	if(FSM)
	{
		// FSM의 IsPlayingAttackAnimation 플래그를 true로 설정합니다.
		FSM->IsPlayingAttackAnimation = true;
		IsPlayingAttackAnimation = true;

		// FSM의 Timeline을 처음부터 재생합니다.
		FSM->Timeline.PlayFromStart();
	}
}

void UEnemyAnim::AnimNotify_AttackEnd()
{
	// 공격 애니메이션이 끝날 때 호출되는 함수입니다.
	if(FSM)
	{   
		// FSM의 Timeline을 정지합니다.
		FSM->Timeline.Stop();

		// FSM의 IsPlayingAttackAnimation 플래그를 false로 설정합니다.
		FSM->IsPlayingAttackAnimation = false;
		IsPlayingAttackAnimation = false;
	}
}

void UEnemyAnim::AnimNotify_DamageEnd()
{
	// 피해 애니메이션이 끝날 때 호출되는 함수입니다.
	// 캐릭터의 움직임을 활성화합니다.
	me->GetCharacterMovement()->Activate();

	// FSM의 상태를 MOVE로 설정합니다.
	if (FSM)
	{
		FSM->State = EEnemyState::MOVE;
	}
}

void UEnemyAnim::AnimNotify_DieEnd()
{
	// 죽음 애니메이션이 끝날 때 호출되는 함수입니다.
	// 캐릭터의 메시에 대해 충돌 응답을 Overlap으로 설정합니다.
	me->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 캐릭터의 메시에 대해 충돌을 쿼리 및 물리적으로 활성화합니다.
	me->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void UEnemyAnim::ReplicateChangedState()
{
	// FSM의 상태가 변경되었을 때 호출되는 함수입니다.
	// FSM의 상태를 현재 상태로 복제합니다.
	state = FSM->State;
}



