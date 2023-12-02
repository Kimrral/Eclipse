// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"

#include "Enemy.h"
#include "EnemyFSM.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEnemyAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	me =  Cast<AEnemy>(TryGetPawnOwner());
}


void UEnemyAnim::AnimNotify_AttackStart()
{
	bIsAttackingAnim=true;
	UEnemyFSM* fsm = Cast<UEnemyFSM>(me->GetDefaultSubobjectByName(FName("enemyFSM")));
	if(fsm)
	{
		me->enemyFSM->Timeline.PlayFromStart();
		me->enemyFSM->bIsAttackReady=false;
	}
}

void UEnemyAnim::AnimNotify_AttackEnd()
{
	bIsAttackingAnim=false;
	UEnemyFSM* fsm = Cast<UEnemyFSM>(me->GetDefaultSubobjectByName(FName("enemyFSM")));
	if(fsm)
	{
		me->enemyFSM->Timeline.Stop();
	}
}

void UEnemyAnim::AnimNotify_DamageEnd()
{
	me->GetCharacterMovement()->Activate();
}

void UEnemyAnim::AnimNotify_DieEnd()
{
	me->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

bool UEnemyAnim::IsAttackAnimationPlaying()
{
	if(bIsAttackingAnim)
	{
		return true;
	}
	return false;
}

