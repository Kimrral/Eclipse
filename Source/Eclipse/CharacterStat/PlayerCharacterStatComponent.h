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

	float ApplyDamage(float InDamage, AActor* DamageCauser);	
	
	FORCEINLINE void HealHp(const float InHealAmount) { CurrentHp = FMath::Clamp(CurrentHp + InHealAmount, 0, MaxHp); OnHpChanged.Broadcast(); }
	
	FORCEINLINE float GetRecoilRate() const {return RecoilRate;}
	void SetRecoilRate(const TArray<bool>& WeaponArray);

	double GenerateRandomFloat(const float InFloat) const;
	int32 GenerateRandomInteger(const float InFloat) const;

	float GetAttackDamage(const TArray<bool>& WeaponArray, const bool IsPlayer) const;	
	
	UFUNCTION(BlueprintCallable)
	void SetHp(float NewHp);

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(Replicated, VisibleInstanceOnly, Category = Score)
	float AccumulatedDamageToEnemy;

	UPROPERTY(Replicated, VisibleInstanceOnly, Category = Score)
	float AccumulatedDamageToPlayer;

protected:
	virtual void InitializeComponent() override;
	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHp, Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHp, Transient, VisibleInstanceOnly, Category = Stat)
	float MaxHp = 100.f;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float RecoilRate;

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float BaseRifleRecoilRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAttackDamageRifle = 9.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAttackDamagePistol = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAttackDamageSniper = 70.f;

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

	UFUNCTION()
	void OnRep_CurrentHp() const;

	UFUNCTION()
	void OnRep_MaxHp() const;
};
