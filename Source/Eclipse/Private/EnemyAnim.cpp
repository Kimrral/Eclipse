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
		bIsAttackingAnim=true;
	}
}

void UEnemyAnim::AnimNotify_AttackEnd()
{
	bIsAttackingAnim=false;
	UEnemyFSM* fsm = Cast<UEnemyFSM>(me->GetDefaultSubobjectByName(FName("enemyFSM")));
	if(fsm)
	{
		me->enemyFSM->Timeline.Stop();
		bIsAttackingAnim=false;
	}
}

void UEnemyAnim::AnimNotify_DamageEnd()
{
	me->GetCharacterMovement()->Activate();
	UEnemyFSM* fsm = Cast<UEnemyFSM>(me->GetDefaultSubobjectByName(FName("enemyFSM")));
	if(fsm)
	{
		me->enemyFSM->state=EEnemyState::MOVE;
	}
}

void UEnemyAnim::AnimNotify_DieEnd()
{
	me->GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
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

