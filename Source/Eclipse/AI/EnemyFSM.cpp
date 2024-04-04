// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"

#include "EclipseAIController.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/Enemy/Enemy.h"
#include "Eclipse/Animation/EnemyAnim.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);

	state = EEnemyState::IDLE;
	me = Cast<AEnemy>(GetOwner());
	AIController = Cast<AEclipseAIController>(me->GetController());

	// Origin Location
	InitialPosition = me->GetActorLocation();

	// Set MoveSpeed
	me->GetCharacterMovement()->MaxWalkSpeed = maxWalkSpeed;

	me->EnemyStat->OnHpZero.AddUObject(this, &UEnemyFSM::DieProcess);
	me->EnemyStat->OnEnemyDamaged.AddUObject(this, &UEnemyFSM::FindAgressivePlayer);

	// Timeline Binding
	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &UEnemyFSM::SetRotToPlayer);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}
}

void UEnemyFSM::InitializeComponent()
{
	Super::InitializeComponent();
}

void UEnemyFSM::ReadyForReplication()
{
	Super::ReadyForReplication();
}

void UEnemyFSM::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEnemyFSM, state);
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
	if (player)
	{
		if (player->IsPlayerDeadImmediately)
		{
			SetState(EEnemyState::MOVE);
			return;
		}
		// 플레이어와 적 간의 거리값 도출
		const auto distToPlayer = player->GetDistanceTo(me);
		if (distToPlayer <= aggressiveRange && me->bPlayerInSight)
		{
			// 탐색 범위 내에 플레이어가 있다면, 이동 상태로 전이
			SetState(EEnemyState::MOVE);
		}
	}
}

void UEnemyFSM::TickMove()
{
	if (player)
	{
		if (player->IsPlayerDeadImmediately)
		{
			MoveBackToInitialPosition();
			return;
		}		
		// 타임라인을 이용한 Enemy 캐릭터 회전 러프
		Timeline.PlayFromStart();
		if (AIController) AIController->MoveToPlayer(player);
		const float dist = player->GetDistanceTo(me);
		if (dist <= attackRange && me->bPlayerInSight)
		{
			if (AIController) AIController->StopMovement();
			// 플레이어가 공격 범위 내에 위치한다면, 공격 상태로 전이
			SetState(EEnemyState::ATTACK);
		}
	}
	else
	{
		SetState(EEnemyState::IDLE);
	}
}

void UEnemyFSM::TickAttack()
{
	if (IsAttackAnimationPlaying())
	{
		return;
	}
	if (player)
	{
		if (player->IsPlayerDeadImmediately)
		{
			SetState(EEnemyState::IDLE);
			return;
		}
		// 플레이어와의 거리 도출
		const float dist = player->GetDistanceTo(me);
		// 공격거리보다 멀어졌다면
		if (dist > attackRange || !me->bPlayerInSight)
		{
			// 이동상태로 전이한다
			SetState(EEnemyState::MOVE);
		}
	}
	else
	{
		SetState(EEnemyState::IDLE);
	}
}

void UEnemyFSM::TickDamage()
{
	// Move 상태로 전이한다.
	SetState(EEnemyState::MOVE);
}

void UEnemyFSM::TickDie()
{
	// 실행되고 있는 Timeline 종료
	Timeline.Stop();
	// Die 함수 1회 호출을 위한 불리언
	if (bTickDie == false)
	{
		bTickDie = true;
		me->OnDie();
	}
}

void UEnemyFSM::DieProcess()
{
	// Die 상태로 전이한다.
	SetState(EEnemyState::DIE);
}

void UEnemyFSM::SetState(EEnemyState next) // 상태 전이함수
{
	if (me->HasAuthority())
	{
		state = next;
		me->EnemyAnim->state = next;
	}	
}

void UEnemyFSM::SetRotToPlayer(float Value)
{
	if (player && me->HasAuthority() && me->EnemyStat->IsStunned==false)
	{
		if (player->IsPlayerDeadImmediately)
		{
			const FVector dir = InitialPosition - me->GetActorLocation();
			// 벡터값에서 회전값 산출
			const FRotator attackRot = UKismetMathLibrary::MakeRotFromXZ(dir, player->GetActorUpVector());
			const FRotator startRot = me->GetActorRotation();
			const FRotator endRot = attackRot;
			// RLerp와 TimeLine Value 값을 통한 자연스러운 회전
			const FRotator lerp = UKismetMathLibrary::RLerp(startRot, endRot, Value, true);
			// 해당 회전값 Enemy에 할당
			me->SetActorRotation(FRotator(0, lerp.Yaw, 0));
		}
		else
		{
			// 플레이어를 바라보는 벡터값 산출
			const FVector dir = player->GetActorLocation() - me->GetActorLocation();
			// 벡터값에서 회전값 산출
			const FRotator attackRot = UKismetMathLibrary::MakeRotFromXZ(dir, player->GetActorUpVector());
			const FRotator startRot = me->GetActorRotation();
			const FRotator endRot = attackRot;
			// RLerp와 TimeLine Value 값을 통한 자연스러운 회전
			const FRotator lerp = UKismetMathLibrary::RLerp(startRot, endRot, Value, true);
			// 해당 회전값 Enemy에 할당
			me->SetActorRotation(FRotator(0, lerp.Yaw, 0));
		}
	}
}

void UEnemyFSM::FindAgressivePlayer()
{
	if (state == EEnemyState::IDLE)
	{
		player = ReturnAgressivePlayer();
		SetState(EEnemyState::MOVE);
		return;
	}
	if (state == EEnemyState::MOVE)
	{
		player = ReturnAgressivePlayer();
	}
}

APlayerCharacter* UEnemyFSM::ReturnAgressivePlayer()
{
	TArray<AActor*> ActorCharacterArray;
	TArray<APlayerCharacter*> PlayerCharacterArray;
	PlayerCharacterArray.Reset();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), ActorCharacterArray);
	int MaxDamageIndex = 0;
	for (int i = 0; i < ActorCharacterArray.Num(); i++)
	{
		if (APlayerCharacter* Player = Cast<APlayerCharacter>(ActorCharacterArray[i]))
		{
			PlayerCharacterArray.Add(Player);
		}
	}
	for (int i = 0; i < PlayerCharacterArray.Num(); i++)
	{
		// [MaxDistIndex]번째 플레이어 누적 데미지
		const float MaxDamage = PlayerCharacterArray[MaxDamageIndex]->Stat->AccumulatedDamageToEnemy;
		// [i]번째 플레이어 누적 데미지
		const float NextDamage = PlayerCharacterArray[i]->Stat->AccumulatedDamageToEnemy;

		// 만약 이번 대상이 현재 대상보다 축적 데미지가 많다면
		if (NextDamage > MaxDamage)
		{
			// 누적 데미지가 많은 대상 플레이어로 변경하기
			MaxDamageIndex = i;
		}
	}
	return PlayerCharacterArray[MaxDamageIndex];
}

void UEnemyFSM::MoveBackToInitialPosition()
{
	if (state == EEnemyState::IDLE)
	{
		return;
	}
	// 타임라인을 이용한 Enemy 캐릭터 회전 러프
	Timeline.PlayFromStart();
	if (AIController) AIController->MoveToLocation(InitialPosition);
	if (FVector::Dist(me->GetActorLocation(), InitialPosition) <= 100.f)
	{
		player=nullptr;
		SetState(EEnemyState::IDLE);
	}
}

void UEnemyFSM::OnRep_EnemyState()
{
	OnStateChanged.Broadcast();
}

bool UEnemyFSM::IsAttackAnimationPlaying()
{
	if (IsPlayingAttackAnimation)
	{
		return true;
	}
	return false;
}
