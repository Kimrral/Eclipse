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
#include "Eclipse/Enemy/Boss.h"
#include "Net/UnrealNetwork.h"

// 이 컴포넌트의 기본 속성 값 설정
UEnemyFSM::UEnemyFSM()
{
	// 게임 시작 시 컴포넌트가 초기화되고, 매 프레임마다 틱을 받도록 설정합니다. 필요 없으면 성능 향상을 위해 끌 수 있습니다.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	// ...
}

// 게임 시작 시 호출됨
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true); // 이 컴포넌트가 네트워크에서 복제되도록 설정

	State = EEnemyState::IDLE; // 초기 상태를 IDLE로 설정
	Me = Cast<AEnemy>(GetOwner()); // 소유자 객체를 AEnemy로 캐스팅
	AIController = Cast<AEclipseAIController>(Me->GetController()); // AI 컨트롤러를 AEclipseAIController로 캐스팅

	// 원래의 위치와 회전 값 저장
	InitialPosition = Me->GetActorLocation();
	InitialRotation = Me->GetActorRotation();

	// 이동 속도 설정
	Me->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

	// 적이 피해를 입었을 때 호출될 콜백 함수 등록
	Me->EnemyStat->OnEnemyDamaged.AddUniqueDynamic(this, &UEnemyFSM::FindAggressivePlayer);

	// 타임라인 바인딩
	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &UEnemyFSM::SetRotToPlayer);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}

	// AI 컨트롤러가 유효하면 랜덤 이동 설정을 바인딩
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

	// 네트워크에서 상태를 복제하도록 설정
	DOREPLIFETIME(UEnemyFSM, State);
}

// 매 프레임 호출됨
void UEnemyFSM::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Timeline.TickTimeline(DeltaTime); // 타임라인 업데이트

	// 상태 머신을 사용하여 현재 상태에 따라 적절한 Tick 함수 호출
	switch (State)
	{
	case EEnemyState::IDLE:
		TickIdle(); // IDLE 상태일 때 처리
		break;
	case EEnemyState::MOVE:
		TickMove(); // MOVE 상태일 때 처리
		break;
	case EEnemyState::ATTACK:
		TickAttack(); // ATTACK 상태일 때 처리
		break;
	case EEnemyState::DAMAGE:
		TickDamage(); // DAMAGE 상태일 때 처리
		break;
	case EEnemyState::DIE:
		TickDie(); // DIE 상태일 때 처리
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
			Player = nullptr; // 플레이어가 죽었으면 플레이어 객체를 null로 설정
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
			MoveBackToInitialPosition(); // 플레이어가 죽으면 초기 위치로 이동
			return;
		}		
		if (const float Dist = Player->GetDistanceTo(Me))
		{
			if (Dist > ChaseLimitRange)
			{
				MoveBackToInitialPosition(); // 플레이어가 추적 범위를 넘어가면 초기 위치로 이동
				return;
			}
			if (Dist > AttackRange && Dist <= ChaseLimitRange)
			{
				if (AIController)
				{
					// 타임라인을 이용하여 적 캐릭터 회전
					Timeline.PlayFromStart();
					AIController->MoveToPlayer(Player);
				}
			}
			if (Dist <= AttackRange)
			{
				if (Me->bPlayerInSight)
				{
					if (AIController) AIController->StopMovement(); // 공격 범위 내에 플레이어가 있으면 이동 중지
					// 공격 상태로 전이
					SetState(EEnemyState::ATTACK);
				}
				else
				{
					if (AIController)
					{
						Timeline.PlayFromStart();
						AIController->MoveToPlayer(Player); // 공격 범위 내에 플레이어가 없으면 다시 이동
					}
				}
			}
		}
		return;
	}
	
	// 초기 위치로 돌아가는 중
	if (IsMovingBack)
	{
		if (AIController) AIController->MoveToLocation(InitialPosition);
		if (FVector::Dist(Me->GetActorLocation(), InitialPosition) < 20.f)
		{
			if (AIController)
			{
				AIController->StopMovement();
				AIController->SetControlRotation(InitialRotation);
			}
			SetState(EEnemyState::IDLE); // 초기 위치에 도착하면 IDLE 상태로 전이
			IsMovingBack = false;
			return;
		}
		return;
	}

	// 랜덤 이동 중
	if (IsMovingRandom)
	{
		if (Me->GetCharacterMovement()->GetCurrentAcceleration().Normalize() == 0.f)
		{
			StuckedTime += GetWorld()->GetDeltaSeconds();
			if (StuckedTime > 3.f)
			{
				SetState(EEnemyState::IDLE); // 이동 중 멈추면 IDLE 상태로 전이
				IsMovingRandom = false;
				StuckedTime = 0.f;
				return;
			}
		}
		if (FVector::Dist(Me->GetActorLocation(), RandomMoveTargetLocation) < 50.f)
		{
			SetState(EEnemyState::IDLE); // 랜덤 이동 목표 위치에 도달하면 IDLE 상태로 전이
			IsMovingRandom = false;
			StuckedTime = 0.f;
		}
	}
}

void UEnemyFSM::TickAttack()
{
	if (IsAttackAnimationPlaying())
	{
		return; // 공격 애니메이션이 재생 중이면 아무 작업도 하지 않음
	}
	if (Player)
	{
		if (Player->IsPlayerDeadImmediately)
		{
			MoveBackToInitialPosition(); // 플레이어가 죽으면 초기 위치로 이동
			return;
		}
		if (Me->bPlayerInSight)
		{
			// 플레이어와의 거리 확인
			if (const float Dist = Player->GetDistanceTo(Me))
			{
				if (Dist > AttackRange)
				{
					// 공격 범위를 넘어가면 이동 상태로 전이
					SetState(EEnemyState::MOVE);
				}
			}
		}
		else
		{
			SetState(EEnemyState::MOVE); // 플레이어가 시야에 없으면 이동 상태로 전이
		}
	}
}

void UEnemyFSM::TickDamage()
{
	// 피해를 입은 후 이동 상태로 전이
	SetState(EEnemyState::MOVE);
}

void UEnemyFSM::TickDie()
{
	// DIE 상태의 Tick 함수 (빈 상태)
}

void UEnemyFSM::RandomMoveSettings(FVector TargetLocation)
{
	if(State == EEnemyState::IDLE)
	{
		IsMovingRandom = true;
		RandomMoveTargetLocation = TargetLocation; // 랜덤 이동 목표 위치 설정
		SetState(EEnemyState::MOVE); // 이동 상태로 전이
	}
}

void UEnemyFSM::SetState(const EEnemyState Next) // 상태 전이 함수
{
	if (Me->HasAuthority())
	{
		State = Next; // 상태를 다음 상태로 설정
		Me->EnemyAnim->state = Next; // 애니메이션 상태를 다음 상태로 설정
	}
}

// 회전 선형 보간을 위한 타임라인 바인딩 함수
void UEnemyFSM::SetRotToPlayer(const float Value)
{
	if (State == EEnemyState::DIE)
	{
		return; // DIE 상태에서는 회전 처리를 하지 않음
	}
	if (Player && Me->HasAuthority() && Me->EnemyStat->IsStunned == false)
	{
		if (Player->IsPlayerDeadImmediately)
		{
			// 플레이어가 죽었으면 초기 위치를 향하는 회전 값 계산
			const FVector Dir = InitialPosition - Me->GetActorLocation();
			const FRotator AttackRot = UKismetMathLibrary::MakeRotFromXZ(Dir, Player->GetActorUpVector());
			const FRotator StartRot = Me->GetActorRotation();
			const FRotator EndRot = AttackRot;
			const FRotator Lerp = UKismetMathLibrary::RLerp(StartRot, EndRot, Value, true);
			Me->SetActorRotation(FRotator(0, Lerp.Yaw, 0)); // 회전 값 적용
		}
		else
		{
			// 플레이어를 향하는 회전 값 계산
			const FVector Dir = Player->GetActorLocation() - Me->GetActorLocation();
			const FRotator AttackRot = UKismetMathLibrary::MakeRotFromXZ(Dir, Player->GetActorUpVector());
			const FRotator StartRot = Me->GetActorRotation();
			const FRotator EndRot = AttackRot;
			const FRotator Lerp = UKismetMathLibrary::RLerp(StartRot, EndRot, Value, true);
			Me->SetActorRotation(FRotator(0, Lerp.Yaw, 0)); // 회전 값 적용
		}
	}
}

void UEnemyFSM::FindAggressivePlayer()
{
	if (State == EEnemyState::IDLE)
	{
		Player = ReturnAggressivePlayer(); // 가장 위협적인 플레이어 찾기
		SetState(EEnemyState::MOVE); // 이동 상태로 전이
		return;
	}
	if (State == EEnemyState::MOVE)
	{
		Player = ReturnAggressivePlayer(); // 가장 위협적인 플레이어 찾기
	}
}

// 가장 위협적인 플레이어를 선별하여 타겟 지정
APlayerCharacter* UEnemyFSM::ReturnAggressivePlayer() const
{
	TArray<AActor*> ActorCharacterArray;
	TArray<APlayerCharacter*> PlayerCharacterArray;
	PlayerCharacterArray.Reset(); // 플레이어 캐릭터 배열 초기화
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), ActorCharacterArray); // 모든 플레이어 캐릭터 가져오기
	int MaxDamageIndex = 0;
	for (AActor* const AggressiveCharacter : ActorCharacterArray)
	{
		if (APlayerCharacter* AggressivePlayer = Cast<APlayerCharacter>(AggressiveCharacter))
		{
			// 추적 범위 내의 캐릭터인지 확인
			if (AggressivePlayer->GetDistanceTo(Me) < ChaseLimitRange)
			{
				PlayerCharacterArray.Add(AggressivePlayer); // 범위 내 플레이어 캐릭터 추가
			}
		}
	}
	// 범위 내 플레이어 중 가장 위협적인 플레이어 찾기
	for (int i = 0; i < PlayerCharacterArray.Num(); i++)
	{
		const float MaxDamage = PlayerCharacterArray[MaxDamageIndex]->Stat->AccumulatedDamageToEnemy;
		const float NextDamage = PlayerCharacterArray[i]->Stat->AccumulatedDamageToEnemy;

		// 현재 플레이어보다 더 많은 데미지를 입힌 플레이어로 변경
		if (NextDamage > MaxDamage)
		{
			MaxDamageIndex = i;
		}
	}
	// 가장 위협적인 플레이어 반환
	if (PlayerCharacterArray.IsValidIndex(MaxDamageIndex))
	{
		return PlayerCharacterArray[MaxDamageIndex];
	}
	
	return nullptr; // 적절한 플레이어가 없으면 nullptr 반환
}

void UEnemyFSM::MoveBackToInitialPosition()
{
	if (State == EEnemyState::IDLE)
	{
		return; // 이미 IDLE 상태면 아무 작업도 하지 않음
	}
	State = EEnemyState::MOVE; // 이동 상태로 전이

	Player = nullptr; // 플레이어 타겟 제거
	IsMovingBack = true; // 초기 위치로 돌아가는 중 상태 설정

	if (AIController) AIController->MoveToLocation(InitialPosition); // 초기 위치로 이동
}

void UEnemyFSM::OnRep_EnemyState() const
{
	OnStateChanged.Broadcast(); // 상태 변경 시 이벤트 발생
}

bool UEnemyFSM::IsAttackAnimationPlaying() const
{
	// 공격 애니메이션이 재생 중인지 여부 반환
	if (IsPlayingAttackAnimation)
	{
		return true;
	}
	return false;
}
