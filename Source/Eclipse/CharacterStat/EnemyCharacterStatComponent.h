// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnEnemyHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEnemyHpChangedDelegate, float, float);
DECLARE_MULTICAST_DELEGATE(FOnEnemyShieldZeroDelegate);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEnemyShieldChangedDelegate, float, float);
DECLARE_MULTICAST_DELEGATE(FOnEnemyDamagedDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ECLIPSE_API UEnemyCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnEnemyHpZeroDelegate OnHpZero;
	FOnEnemyHpChangedDelegate OnHpChanged;
	FOnEnemyShieldZeroDelegate OnShieldZero;
	FOnEnemyShieldChangedDelegate OnShieldChanged;
	FOnEnemyDamagedDelegate OnEnemyDamaged;


	// Sets default values for this component's properties
	UEnemyCharacterStatComponent();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHp() const { return CurrentEnemyHp; }

	FORCEINLINE float GetCurrentShield() const { return CurrentEnemyShield; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHp() const { return MaxEnemyHp; }

	FORCEINLINE float GetMaxShield() const { return MaxEnemyShield; }

	float ApplyDamage(float InDamage, AActor* DamageCauser);
	float ApplyShieldDamage(float InShieldDamage, AActor* DamageCauser);
	UFUNCTION(BlueprintCallable)
	void SetHp(float NewHp);
	void SetShield(float NewShield);

	UPROPERTY(Replicated, VisibleInstanceOnly, Category = Stat)
	bool IsStunned = false;

	UPROPERTY(Replicated, VisibleInstanceOnly, Category = Stat)
	bool IsShieldBroken;

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

protected:
	virtual void InitializeComponent() override;
	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void InitShieldBoolean();

	UPROPERTY(ReplicatedUsing = OnRep_CurrentEnemyHp, Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentEnemyHp;

	UPROPERTY(ReplicatedUsing = OnRep_MaxEnemyHp, EditDefaultsOnly, Category = Stat)
	float MaxEnemyHp = 1000.f;


	UPROPERTY(ReplicatedUsing = OnRep_CurrentEnemyShield, Transient, VisibleInstanceOnly, Category = Stat)
	int CurrentEnemyShield;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentEnemyShield, EditDefaultsOnly, Category = Stat)
	int MaxEnemyShield = 100.f;

	UFUNCTION()
	void OnRep_CurrentEnemyHp() const;

	UFUNCTION()
	void OnRep_MaxEnemyHp() const;

	UFUNCTION()
	void OnRep_CurrentEnemyShield() const;

	UFUNCTION()
	void OnRep_MaxEnemyShield() const;
};
