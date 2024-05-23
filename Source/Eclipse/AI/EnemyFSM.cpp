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
	Me->EnemyStat->OnEnemyDamaged.AddUObject(this, &UEnemyFSM::FindAggressivePlayer);

	// Timeline Binding
	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &UEnemyFSM::SetRotToPlayer);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}

	if (AIController)
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
	if (Player)
	{
		if (Player->IsPlayerDeadImmediately)
		{
			MoveBackToInitialPosition();
			return;
		}		
		if (const float Dist = Player->GetDistanceTo(Me))
		{
			if (Dist > ChaseLimitRange)
			{
				MoveBackToInitialPosition();
				return;
			}
			if(Dist>AttackRange && Dist <= ChaseLimitRange)
			{
				if (AIController)
				{
					// 타임라인을 이용한 Enemy 캐릭터 회전 러프
					Timeline.PlayFromStart();
					AIController->MoveToPlayer(Player);
				}
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

	if (IsMovingRandom)
	{
		if (Me->GetCharacterMovement()->GetCurrentAcceleration().Normalize() == 0.f)
		{
			StuckedTime += GetWorld()->GetDeltaSeconds();
			if (StuckedTime > 3.f)
			{
				SetState(EEnemyState::IDLE);
				IsMovingRandom = false;
				StuckedTime = 0.f;
				return;
			}
		}
		if (FVector::Dist(Me->GetActorLocation(), RandomMoveTargetLocation) < 50.f)
		{
			SetState(EEnemyState::IDLE);
			IsMovingRandom = false;
			StuckedTime = 0.f;
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
	if(State==EEnemyState::IDLE)
	{
		IsMovingRandom = true;
		RandomMoveTargetLocation = TargetLocation;
		SetState(EEnemyState::MOVE);
	}
}

void UEnemyFSM::SetState(const EEnemyState Next) // 상태 전이함수
{
	if (Me->HasAuthority())
	{
		State = Next;
		Me->EnemyAnim->state = Next;
	}
}

// 회전 선형보간을 위한 타임라인 바인딩 함수
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
			// 초기 스폰 위치를 향하는 벡터값 산출
			const FVector Dir = InitialPosition - Me->GetActorLocation();
			// 초기 스폰 위치를 향하는 방향벡터에서 회전값 산출
			const FRotator AttackRot = UKismetMathLibrary::MakeRotFromXZ(Dir, Player->GetActorUpVector());
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
			// 플레이어를 향하는 방향벡터에서 회전값 산출
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

void UEnemyFSM::FindAggressivePlayer()
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

// 가장 위협적인 플레이어 선별과 타겟 지정
APlayerCharacter* UEnemyFSM::ReturnAggressivePlayer() const
{
	TArray<AActor*> ActorCharacterArray;
	TArray<APlayerCharacter*> PlayerCharacterArray;
	// 함수 호출마다 캐릭터 배열 리셋
	PlayerCharacterArray.Reset();
	// 월드 내 모든 플레이어 캐릭터 순회
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), ActorCharacterArray);
	int MaxDamageIndex = 0;
	for (AActor* const AggressiveCharacter : ActorCharacterArray)
	{
		if (APlayerCharacter* AggressivePlayer = Cast<APlayerCharacter>(AggressiveCharacter))
		{
			// 추적 범위 내의 캐릭터 여부 우선 판별
			if(AggressivePlayer->GetDistanceTo(Me)<ChaseLimitRange)
			{
				// 추적 범위 내에 있다면, PlayerCharacterArray에 추가
				PlayerCharacterArray.Add(AggressivePlayer);
			}
		}
	}
	// 범위 내 플레이어 중 가장 위협적인 대상 선별
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
	// 인덱스 유효 여부 검사 후 리턴
	if(PlayerCharacterArray.IsValidIndex(MaxDamageIndex))
	{
		return PlayerCharacterArray[MaxDamageIndex];
	}
	
	return nullptr;
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
