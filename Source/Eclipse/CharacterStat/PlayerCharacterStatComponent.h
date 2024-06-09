// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "PlayerCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHpChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnStatChangedDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ECLIPSE_API UPlayerCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;
	FOnStatChangedDelegate OnStatChanged;

	// Sets default values for this component's properties
	UPlayerCharacterStatComponent();
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHp() const { return MaxHp; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void HealHp(const float InHealAmount)
	{
		CurrentHp = FMath::Clamp(CurrentHp + InHealAmount, 0, MaxHp);
		OnHpChanged.Broadcast();
	}

	UFUNCTION(BlueprintCallable)
	void SetHp(float NewHp);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void AddMaxHp(const float AddMaxHpAmount)
	{
		MaxHp = FMath::Clamp(MaxHp + AddMaxHpAmount, 0, MaxHp + AddMaxHpAmount);
		HealHp(AddMaxHpAmount);
		OnHpChanged.Broadcast();
	}

	UFUNCTION(BlueprintCallable)
	void SubtractMaxHp(const float SubtractMaxHpAmount);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentRouble() const { return CurrentRouble; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void AddRouble(const float InRoubleAmount) { CurrentRouble += InRoubleAmount; }

	float ApplyDamage(float InDamage, AActor* DamageCauser);

	FORCEINLINE float GetPitchRecoilRate() const { return PitchRecoilRate; }
	FORCEINLINE float GetYawRecoilRate() const { return YawRecoilRate; }

	void SetRecoilRate(const TArray<bool>& WeaponArray);

	FORCEINLINE double GenerateRandomPitchRecoilRate(const float InFloat) const
	{
		const double DoubleRandFloat = FMath::FRandRange(InFloat * 0.8, InFloat * 1.2);
		return DoubleRandFloat;
	}

	FORCEINLINE double GenerateRandomYawRecoilRate(const float InFloat) const
	{
		const double DoubleRandFloat = FMath::FRandRange(InFloat * -1, InFloat);
		return DoubleRandFloat;
	}

	int32 GenerateRandomInteger(const float InFloat) const;

	float GetFireInterval(const TArray<bool>& WeaponArray) const;

	float GetAttackDamage(const TArray<bool>& WeaponArray, const bool IsPlayer) const;

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(Replicated, VisibleInstanceOnly, Category = Score)
	float AccumulatedDamageToEnemy;

	UPROPERTY(Replicated, VisibleInstanceOnly, Category = Score)
	float AccumulatedDamageToBoss;

	UPROPERTY(Replicated, VisibleInstanceOnly, Category = Score)
	float AccumulatedDamageToPlayer;

	UPROPERTY(Replicated, VisibleInstanceOnly, Category = Stat)
	float RecoilStatMultiplier = 1.f;

	UPROPERTY(Replicated, VisibleInstanceOnly, Category = Stat)
	float FireIntervalStatMultiplier = 1.f;
	
	UPROPERTY(Replicated, VisibleInstanceOnly, Category = Stat)
	float DamageStatMultiplier = 1.f;

protected:
	virtual void InitializeComponent() override;
	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHp, Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHp, Transient, VisibleInstanceOnly, Category = Stat)
	float MaxHp = 100.f;

	UPROPERTY(Replicated, VisibleInstanceOnly, Category = Stat)
	float CurrentRouble;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float PitchRecoilRate;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float YawRecoilRate;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float BaseRiflePitchRecoilRate = -0.5f;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float BaseSniperPitchRecoilRate = -1.1f;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float BasePistolPitchRecoilRate = -1.1f;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float BaseM249PitchRecoilRate = -0.6f;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float BaseRifleYawRecoilRate = 0.4f;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float BaseSniperYawRecoilRate = 0.7f;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float BasePistolYawRecoilRate = 0.7f;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float BaseM249YawRecoilRate = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAttackDamageRifle = 9.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAttackDamagePistol = 19.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAttackDamageSniper = 72.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAttackDamageM249 = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemyAttackDamageRifle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemyAttackDamagePistol = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemyAttackDamageSniper = 700.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemyAttackDamageM249 = 80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecRifle = 0.09f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecPistol = 0.33f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecSniper = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecM249 = 0.07f;

	UFUNCTION()
	void OnRep_CurrentHp() const;

	UFUNCTION()
	void OnRep_MaxHp() const;
};
