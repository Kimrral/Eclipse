// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class ECLIPSE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:	
	// Enemy Stat
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyCharacterStatComponent> EnemyStat;
	
	// Reward Manager
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class URewardManagerComponent> RewardManager;
	
	UPROPERTY()
	class AEclipseGameMode* GameMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FSM)
	class UEnemyFSM* EnemyFSM;

	UPROPERTY(EditAnywhere, Category=EnemySettings)
	UAnimMontage* DamageMontage;

	UPROPERTY(EditAnywhere, Category=EnemySettings)
	UAnimMontage* StunMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=EnemySettings)
	TObjectPtr<class UMaterialInterface> HitOverlayMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=EnemySettings)
	TObjectPtr<class UMaterialInterface> HitOverlayMatShield;

	UPROPERTY()
	class UEnemyAnim* EnemyAnim;

	UPROPERTY(EditAnywhere)
	class UPawnSensingComponent* PawnSensingComponent;

	UFUNCTION()
	void OnDie();

	UFUNCTION()
	void OnPawnDetected(APawn* Pawn); 

	UFUNCTION()
	void Damaged(int Damage, AActor* DamageCauser);

	UFUNCTION(Reliable, Server, WithValidation)
	void DamagedRPCServer(int Damage, AActor* DamageCauser);

	UFUNCTION(Unreliable, NetMulticast)
	void DamagedRPCMulticast(int Damage, AActor* DamageCauser);

	UFUNCTION()
	void ResetOverlayMaterial() const;

	UFUNCTION()
	void OnShieldDestroy();

	UFUNCTION()
	virtual void OnDestroy();

	UFUNCTION(Server, Reliable)
	virtual void FireProcess() const;

	UFUNCTION()
	virtual void SetDissolveValue(float Value);

	UFUNCTION()
	virtual void SetDissolveMaterial();

	UPROPERTY()
	FTimerHandle StunHandle;

	UPROPERTY()
	FTimerHandle HPWidgetInvisibleHandle;

	UPROPERTY()
	class AEclipsePlayerController* PC;	

	TArray<uint32> DynamicMaterialIndices;

	UPROPERTY()
	bool bDeath = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayerInSight = false;

	UPROPERTY(EditAnywhere) // Timeline 생성
	FTimeline DissolveTimeline;

	UPROPERTY(EditAnywhere) // Timeline 커브
	UCurveFloat* DissolveCurveFloat;
};
