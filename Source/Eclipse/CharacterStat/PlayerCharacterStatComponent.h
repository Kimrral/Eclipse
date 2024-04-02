// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "PlayerCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHpChangedDelegate);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ECLIPSE_API UPlayerCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;


	// Sets default values for this component's properties
	UPlayerCharacterStatComponent();
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHp() const { return MaxHp; }

	float ApplyDamage(float InDamage, AActor* DamageCauser);
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


	UFUNCTION()
	void OnRep_CurrentHp();

	UFUNCTION()
	void OnRep_MaxHp();
};
