// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterStatComponent.h"

#include "Eclipse/Player/EclipsePlayerController.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPlayerCharacterStatComponent::UPlayerCharacterStatComponent()
{
	bWantsInitializeComponent = true;
}

void UPlayerCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetHp(MaxHp);
	SetIsReplicated(true);
}


float UPlayerCharacterStatComponent::ApplyDamage(const float InDamage, AActor* DamageCauser)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage);
	if (CurrentHp <= 0.0f)
	{
		OnHpZero.Broadcast();

		PlayerCharacter = Cast<APlayerCharacter>(DamageCauser);
		if (PlayerCharacter)
		{
			PlayerCharacter->OnPlayerKill();
		}
	}
	return ActualDamage;
}

void UPlayerCharacterStatComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
}

void UPlayerCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerCharacterStatComponent, CurrentHp);
	DOREPLIFETIME(UPlayerCharacterStatComponent, MaxHp);
	DOREPLIFETIME(UPlayerCharacterStatComponent, AccumulatedDamageToPlayer);
	DOREPLIFETIME(UPlayerCharacterStatComponent, AccumulatedDamageToEnemy);
	DOREPLIFETIME(UPlayerCharacterStatComponent, CurrentRouble);
}

void UPlayerCharacterStatComponent::SetRecoilRate(const TArray<bool>& WeaponArray)
{
	if (WeaponArray[0] == true)
	{
		RecoilRate = GenerateRandomFloat(BaseRifleRecoilRate);
		return;
	}
	if (WeaponArray[1] == true)
	{
		return;
	}
	if (WeaponArray[2] == true)
	{
		return;
	}
	if (WeaponArray[3] == true)
	{
		return;
	}
}

double UPlayerCharacterStatComponent::GenerateRandomFloat(const float InFloat) const
{
	const double DoubleRandFloat = FMath::FRandRange(InFloat * 0.8, InFloat * 1.2);
	return DoubleRandFloat;
}

int32 UPlayerCharacterStatComponent::GenerateRandomInteger(const float InFloat) const
{
	const double DoubleRandInteger = FMath::FRandRange(InFloat * 0.8, InFloat * 1.2);
	const int32 RoundedRandInteger = FMath::RoundHalfToEven(DoubleRandInteger);
	return RoundedRandInteger;
}

void UPlayerCharacterStatComponent::SetHp(const float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);
}


void UPlayerCharacterStatComponent::OnRep_CurrentHp() const
{
	OnHpChanged.Broadcast();
	if (CurrentHp <= 0.0f)
	{
		OnHpZero.Broadcast();
	}
}

void UPlayerCharacterStatComponent::OnRep_MaxHp() const
{
	OnHpChanged.Broadcast();
}

float UPlayerCharacterStatComponent::GetAttackDamage(const TArray<bool>& WeaponArray, const bool IsPlayer) const
{
	if (WeaponArray[0] == true)
	{
		if (IsPlayer)
		{
			return GenerateRandomInteger(PlayerAttackDamageRifle);
		}
		return GenerateRandomInteger(EnemyAttackDamageRifle);
	}
	if (WeaponArray[1] == true)
	{
		if (IsPlayer)
		{			
			return GenerateRandomInteger(PlayerAttackDamageSniper);
		}		
		return GenerateRandomInteger(EnemyAttackDamageSniper);
	}
	if (WeaponArray[2] == true)
	{
		if (IsPlayer)
		{			
			return GenerateRandomInteger(PlayerAttackDamagePistol);
		}
		return GenerateRandomInteger(EnemyAttackDamagePistol);
	}
	if (WeaponArray[3] == true)
	{
		if (IsPlayer)
		{
			return GenerateRandomInteger(PlayerAttackDamageM249);
		}
		return GenerateRandomInteger(EnemyAttackDamageM249);
	}
	return 0;
}
