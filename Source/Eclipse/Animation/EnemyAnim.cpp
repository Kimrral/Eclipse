// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"

#include "Eclipse/Enemy/Enemy.h"
#include "Eclipse/AI/EnemyFSM.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemyAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	me = Cast<AEnemy>(TryGetPawnOwner());
	FSM = Cast<UEnemyFSM>(me->GetDefaultSubobjectByName(FName("enemyFSM")));

	FSM->OnStateChanged.AddUObject(this, &UEnemyAnim::ReplicateChangedState);
}


void UEnemyAnim::AnimNotify_AttackStart()
{
	if(FSM)
	{
		FSM->IsPlayingAttackAnimation = true;
		IsPlayingAttackAnimation=true;
		FSM->Timeline.PlayFromStart();		
	}
}

void UEnemyAnim::AnimNotify_AttackEnd()
{
	if(FSM)
	{		
		FSM->Timeline.Stop();
		FSM->IsPlayingAttackAnimation = false;
		IsPlayingAttackAnimation=false;
	}
}

void UEnemyAnim::AnimNotify_DamageEnd()
{
	me->GetCharacterMovement()->Activate();
	if (FSM)
	{
		FSM->state = EEnemyState::MOVE;
	}
}

void UEnemyAnim::AnimNotify_DieEnd()
{
	me->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	me->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void UEnemyAnim::ReplicateChangedState()
{
	state= FSM->state;
}



