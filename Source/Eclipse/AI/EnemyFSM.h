// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "EnemyFSM.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnStateChangeDele);

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	IDLE,
	MOVE,
	ATTACK,
	DAMAGE,
	DIE,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ECLIPSE_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnStateChangeDele OnStateChanged;

	UFUNCTION()
	void TickIdle();

	UFUNCTION()
	void TickMove();

	UFUNCTION()
	void TickAttack();

	UFUNCTION()
	void TickDamage();

	UFUNCTION()
	void TickDie();

	UFUNCTION()
	void DieProcess();

	UFUNCTION()
	void RandomMoveSettings(FVector TargetLocation);

	UFUNCTION()
	void SetState(EEnemyState Next);

	UFUNCTION()
	void SetRotToPlayer(float Value);

	UFUNCTION()
	void FindAggressivePlayer();

	UFUNCTION()
	bool IsAttackAnimationPlaying() const;

	UFUNCTION()
	APlayerCharacter* ReturnAggressivePlayer() const;

	UFUNCTION()
	void MoveBackToInitialPosition();

	UFUNCTION()
	void OnRep_EnemyState() const;
	
	UPROPERTY(ReplicatedUsing=OnRep_EnemyState)
	EEnemyState State;

	UPROPERTY()
	class APlayerCharacter* Player;

	UPROPERTY()
	class AEnemy* Me;

	UPROPERTY()
	class AEclipseAIController* AIController;

	UPROPERTY(VisibleAnywhere)
	bool IsPlayingAttackAnimation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=EnemySettings)
	float ChaseLimitRange = 900.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=EnemySettings)
	float AggressiveRange = 600.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=EnemySettings)
	float AttackRange = 300.f;

	UPROPERTY()
	float CurTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=EnemySettings)
	float AttackDelayTime = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=EnemySettings)
	float MaxWalkSpeed = 200.0f;

	UPROPERTY()
	bool IsMovingBack = false;

	UPROPERTY()
	bool IsMovingRandom = false;

	UPROPERTY()
	float StuckedTime = 0;

	UPROPERTY()
	FVector InitialPosition;

	UPROPERTY()
	FRotator InitialRotation;

	UPROPERTY()
	FVector RandomMoveTargetLocation;

	UPROPERTY()
	float alpha;

	UPROPERTY(EditAnywhere) // Timeline 생성
	FTimeline Timeline;

	UPROPERTY(EditAnywhere) // Timeline 커브
	UCurveFloat* CurveFloat;

	UPROPERTY()
	bool bIsAttackReady;
};
