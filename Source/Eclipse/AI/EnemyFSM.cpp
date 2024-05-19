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

	State = EEnemyState::IDLE;
	Me = Cast<AEnemy>(GetOwner());
	AIController = Cast<AEclipseAIController>(Me->GetController());

	// Origin Transform
	InitialPosition = Me->GetActorLocation();
	InitialRotation = Me->GetActorRotation();

	// Set MoveSpeed
	Me->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

	Me->EnemyStat->OnHpZero.AddUObject(this, &UEnemyFSM::DieProcess);
	Me->EnemyStat->OnEnemyDamaged.AddUObject(this, &UEnemyFSM::FindAgressivePlayer);

	// Timeline Binding
	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &UEnemyFSM::SetRotToPlayer);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}

	if(AIController)
	{
		AIController->AIControllerRandMoveDelegate.BindDynamic(this, &UEnemyFSM::RandomMoveSettings);
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

	DOREPLIFETIME(UEnemyFSM, State);
}

// Called every frame
void UEnemyFSM::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Timeline.TickTimeline(DeltaTime);

	// switch / case 문을 이용한 상태머신
	switch (State)
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
	// 플레이어가 감지되었다면
	if (Player)
	{
		if (Player->IsPlayerDeadImmediately)
		{
			Player = nullptr;
			return;
		}
		// 이동 상태로 전이
		SetState(EEnemyState::MOVE);
	}
}

void UEnemyFSM::TickMove()
{
	if (IsMovingBack)
	{
		if (AIController) AIController->MoveToLocation(InitialPosition);
		if (FVector::Dist(Me->GetActorLocation(), InitialPosition) < 100.f)
		{
			if (AIController)
			{
				AIController->StopMovement();
				AIController->SetControlRotation(InitialRotation);
			}
			SetState(EEnemyState::IDLE);
			IsMovingBack = false;
			return;
		}
		return;
	}
	
	if (Player)
	{
		if (Player->IsPlayerDeadImmediately)
		{
			MoveBackToInitialPosition();
			return;
		}
		// 타임라인을 이용한 Enemy 캐릭터 회전 러프
		Timeline.PlayFromStart();
		if (AIController) AIController->MoveToPlayer(Player);
		if (const float Dist = Player->GetDistanceTo(Me))
		{
			if (Dist >= ChaseLimitRange)
			{
				MoveBackToInitialPosition();
				return;
			}
			if (Dist <= AttackRange)
			{
				if (AIController) AIController->StopMovement();
				// 플레이어가 공격 범위 내에 위치한다면, 공격 상태로 전이
				SetState(EEnemyState::ATTACK);
			}
		}
		return;
	}

	if(IsMovingRandom)
	{
		if(FVector::Dist(Me->GetActorLocation(), RandomMoveTargetLocation) < 100.f)
		{
			SetState(EEnemyState::IDLE);
			IsMovingRandom = false;
		}
	}
}

void UEnemyFSM::TickAttack()
{
	if (IsAttackAnimationPlaying())
	{
		return;
	}
	if (Player)
	{
		if (Player->IsPlayerDeadImmediately)
		{
			MoveBackToInitialPosition();
			return;
		}
		// 플레이어와의 거리 도출
		if (const float Dist = Player->GetDistanceTo(Me))
		{
			// 추적 범위보다 멀어졌다면
			if (Dist > ChaseLimitRange)
			{
				MoveBackToInitialPosition();
				return;
			}
			// 공격거리보다 멀어졌다면
			if (Dist > AttackRange)
			{
				// 이동상태로 전이한다
				SetState(EEnemyState::MOVE);
			}
		}
	}
}

void UEnemyFSM::TickDamage()
{
	// Move 상태로 전이한다.
	SetState(EEnemyState::MOVE);
}

void UEnemyFSM::TickDie()
{
}

void UEnemyFSM::DieProcess()
{
	// Die 상태로 전이한다.
	SetState(EEnemyState::DIE);
	Me->OnDie();
}

void UEnemyFSM::RandomMoveSettings(FVector TargetLocation)
{
	IsMovingRandom = true;
	RandomMoveTargetLocation = TargetLocation;
	SetState(EEnemyState::MOVE);
}

void UEnemyFSM::SetState(EEnemyState Next) // 상태 전이함수
{
	if (Me->HasAuthority())
	{
		State = Next;
		Me->EnemyAnim->state = Next;
	}
}

void UEnemyFSM::SetRotToPlayer(const float Value)
{
	if (State == EEnemyState::DIE)
	{
		return;
	}
	if (Player && Me->HasAuthority() && Me->EnemyStat->IsStunned == false)
	{
		if (Player->IsPlayerDeadImmediately)
		{
			const FVector dir = InitialPosition - Me->GetActorLocation();
			// 벡터값에서 회전값 산출
			const FRotator AttackRot = UKismetMathLibrary::MakeRotFromXZ(dir, Player->GetActorUpVector());
			const FRotator StartRot = Me->GetActorRotation();
			const FRotator EndRot = AttackRot;
			// RLerp와 TimeLine Value 값을 통한 자연스러운 회전
			const FRotator Lerp = UKismetMathLibrary::RLerp(StartRot, EndRot, Value, true);
			// 해당 회전값 Enemy에 할당
			Me->SetActorRotation(FRotator(0, Lerp.Yaw, 0));
		}
		else
		{
			// 플레이어를 바라보는 벡터값 산출
			const FVector Dir = Player->GetActorLocation() - Me->GetActorLocation();
			// 벡터값에서 회전값 산출
			const FRotator AttackRot = UKismetMathLibrary::MakeRotFromXZ(Dir, Player->GetActorUpVector());
			const FRotator StartRot = Me->GetActorRotation();
			const FRotator EndRot = AttackRot;
			// RLerp와 TimeLine Value 값을 통한 자연스러운 회전
			const FRotator Lerp = UKismetMathLibrary::RLerp(StartRot, EndRot, Value, true);
			// 해당 회전값 Enemy에 할당
			Me->SetActorRotation(FRotator(0, Lerp.Yaw, 0));
		}
	}
}

void UEnemyFSM::FindAgressivePlayer()
{
	if (State == EEnemyState::IDLE)
	{
		Player = ReturnAggressivePlayer();
		SetState(EEnemyState::MOVE);
		return;
	}
	if (State == EEnemyState::MOVE)
	{
		Player = ReturnAggressivePlayer();
	}
}

APlayerCharacter* UEnemyFSM::ReturnAggressivePlayer()
{
	TArray<AActor*> ActorCharacterArray;
	TArray<APlayerCharacter*> PlayerCharacterArray;
	PlayerCharacterArray.Reset();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), ActorCharacterArray);
	int MaxDamageIndex = 0;
	for (int i = 0; i < ActorCharacterArray.Num(); i++)
	{
		if (APlayerCharacter* AggressivePlayer = Cast<APlayerCharacter>(ActorCharacterArray[i]))
		{
			PlayerCharacterArray.Add(AggressivePlayer);
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
	if (State == EEnemyState::IDLE)
	{
		return;
	}
	State = EEnemyState::MOVE;

	Player = nullptr;
	IsMovingBack = true;

	if (AIController) AIController->MoveToLocation(InitialPosition);
}

void UEnemyFSM::OnRep_EnemyState() const
{
	OnStateChanged.Broadcast();
}

bool UEnemyFSM::IsAttackAnimationPlaying() const
{
	if (IsPlayingAttackAnimation)
	{
		return true;
	}
	return false;
}
