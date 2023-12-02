// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"

#include "Enemy.h"
#include "EnemyAnim.h"
#include "EnemyHPWidget.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	state = EEnemyState::IDLE;
	me = Cast<AEnemy>(GetOwner());

	// Origin Location
	originPosition = me->GetActorLocation();

	// Set MoveSpeed
	me->GetCharacterMovement()->MaxWalkSpeed=maxWalkSpeed;

	// Timeline Binding
	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &UEnemyFSM::SetRotToPlayer);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}
	
}

// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Timeline.TickTimeline(DeltaTime);

	// switch / case 문을 이용한 상태머신
	switch (state)
	{
	case EEnemyState::IDLE:
		TickIdle();
		break;
	case EEnemyState::MOVE:
		TickMove();
		break;
	case EEnemyState::ATTACK:
		TickAttack();
		break;
	case EEnemyState::DAMAGE:
		TickDamage();
		break;
	case EEnemyState::DIE:
		TickDie();
		break;
	}	
}

void UEnemyFSM::TickIdle()
{
	// 월드 내의 플레이어 찾기
	player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	// 플레이어와 적 간의 거리값 도출
	auto distToPlayer = player->GetDistanceTo(me);
	if(player&&distToPlayer<=aggressiveRange)
	{
		// 탐색 범위 내에 플레이어가 있다면, 이동 상태로 전이
		SetState(EEnemyState::MOVE);
	}
}

void UEnemyFSM::TickMove()
{
	// 플레이어 방향벡터 산출
	FVector dir = player->GetActorLocation() - me->GetActorLocation();
	// 타임라인을 이용한 Enemy 캐릭터 회전 러프
	Timeline.PlayFromStart();
	// 구한 값을 기준으로 이동 인풋
	me->AddMovementInput(dir.GetSafeNormal());	
	float dist = player->GetDistanceTo(me);
	if(dist<=attackRange)
	{
		// 플레이어가 공격 범위 내에 위치한다면, 공격 상태로 전이
		SetState(EEnemyState::ATTACK);
	}
}

void UEnemyFSM::TickAttack()
{
	// curTime에 DeltaTime 누적
	curTime+=GetWorld()->DeltaTimeSeconds;
	if(!bIsAttacking&&curTime>=0.1f)
	{
		me->enemyAnim->bIsAttacking=true;
		bIsAttacking=true;
		float dist = player->GetDistanceTo(me);
		{
			if(dist<=attackRange)
			{
				if(player)
				{
					
				}
			}
		}
	}
	if(me->enemyAnim->IsAttackAnimationPlaying()==false)
	{
		// 플레이어와의 거리 도출
		float dist = player->GetDistanceTo(me);
		// 공격거리보다 멀어졌다면
		if(dist>attackRange)
		{
			// 이동상태로 전이한다
			SetState(EEnemyState::MOVE);
		}
		else
		{
			curTime=0;
			bIsAttacking=false;
			me->enemyAnim->bIsAttacking=false;
		}
	}
}

void UEnemyFSM::TickDamage()
{
	curTime+=GetWorld()->GetDeltaSeconds();
	// Damage 상태로 전이하고 1.0초가 경과하면
	if(curTime>0.0f)
	{
		// Move 상태로 전이한다.
		SetState(EEnemyState::MOVE);
		// 시간 누적 변수 초기화
		curTime=0;
	}
}

void UEnemyFSM::TickDie()
{
	// 실행되고 있는 Timeline 종료
	Timeline.Stop();
	// Die 함수 1회 호출을 위한 불리언
	if(bTickDie==false)
	{
		bTickDie=true;
		me->OnDie();
	}
}

void UEnemyFSM::OnDamageProcess(int damageValue)
{
	// 매개변수 damageValue의 값만큼 현재 HP에서 차감한다.
	me->curHP=FMath::Clamp(me->curHP-=damageValue, 0, me->maxHP);
	if(me->curHP<=0)
	{
		// 해당 HP값을 위젯 머터리얼 파라미터 값에 할당한다.
		me->enemyHPWidget->HPdynamicMat->SetScalarParameterValue(FName("HPAlpha"), 0);
		// Die 상태로 전이한다.
		SetState(EEnemyState::DIE);
	}
	else
	{
		// 해당 HP값을 위젯 머터리얼 파라미터 값에 할당한다.
		me->enemyHPWidget->HPdynamicMat->SetScalarParameterValue(FName("HPAlpha"), me->curHP*0.01-0.001);
		// Damage 상태로 전이한다.
		SetState(EEnemyState::DAMAGE);
	}
	UE_LOG(LogTemp, Warning, TEXT("Enemy HP : %d"), me->curHP);}

void UEnemyFSM::SetState(EEnemyState next) // 상태 전이함수
{
	state=next;
	me->enemyAnim->state=next;
}

void UEnemyFSM::SetRotToPlayer(float Value)
{
	if(player)
	{
		// 플레이어를 바라보는 벡터값 산출
		FVector dir = player->GetActorLocation() - me->GetActorLocation();
		// 벡터값에서 회전값 산출
		FRotator attackRot = UKismetMathLibrary::MakeRotFromXZ(dir, player->GetActorUpVector());
		auto startRot = me->GetActorRotation();
		auto endRot = attackRot;
		// RLerp와 TimeLine Value 값을 통한 자연스러운 회전
		auto lerp = UKismetMathLibrary::RLerp(startRot, endRot, Value, true);
		// 해당 회전값 Enemy에 할당
		me->SetActorRotation(FRotator(0, lerp.Yaw, 0));
	}
}
