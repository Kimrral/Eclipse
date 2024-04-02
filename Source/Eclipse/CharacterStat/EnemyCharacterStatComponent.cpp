// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UEnemyCharacterStatComponent::UEnemyCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UEnemyCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	InitShieldBoolean();
	SetHp(MaxEnemyHp);
	SetShield(MaxEnemyShield);
	SetIsReplicated(true);
}


float UEnemyCharacterStatComponent::ApplyDamage(float InDamage, AActor* DamageCauser)
{
	if (!IsShieldBroken)
	{
		const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);
		ApplyShieldDamage(InDamage * 0.05f, DamageCauser);
		return ActualDamage;
	}
	if (IsStunned)
	{
		InDamage *= 2;
	}
	const float PrevHp = CurrentEnemyHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage);
	if (CurrentEnemyHp <= 0.0f)
	{
		OnHpZero.Broadcast();
		PlayerCharacter = Cast<APlayerCharacter>(DamageCauser);
		if (PlayerCharacter)
		{
			PlayerCharacter->OnEnemyKill();
		}
	}
	return ActualDamage;
}

float UEnemyCharacterStatComponent::ApplyShieldDamage(float InShieldDamage, AActor* DamageCauser)
{
	const float PrevShield = CurrentEnemyShield;
	const float ActualShieldDamage = FMath::Clamp<float>(InShieldDamage, 0, InShieldDamage);

	SetShield(PrevShield - ActualShieldDamage);
	if (CurrentEnemyShield <= 0.0f)
	{
		OnShieldZero.Broadcast();
	}
	return ActualShieldDamage;
}

void UEnemyCharacterStatComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
}

void UEnemyCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEnemyCharacterStatComponent, CurrentEnemyHp);
	DOREPLIFETIME(UEnemyCharacterStatComponent, MaxEnemyHp);
	DOREPLIFETIME(UEnemyCharacterStatComponent, CurrentEnemyShield);
	DOREPLIFETIME(UEnemyCharacterStatComponent, MaxEnemyShield);
	DOREPLIFETIME(UEnemyCharacterStatComponent, IsShieldBroken);
	DOREPLIFETIME(UEnemyCharacterStatComponent, IsStunned);
}

void UEnemyCharacterStatComponent::InitShieldBoolean()
{
	if (GetMaxShield() == 0.0f)
	{
		IsShieldBroken = true;
	}
	else
	{
		IsShieldBroken = false;
	}
}

void UEnemyCharacterStatComponent::SetHp(float NewHp)
{
	CurrentEnemyHp = FMath::Clamp<float>(NewHp, 0.0f, MaxEnemyHp);
}

void UEnemyCharacterStatComponent::SetShield(float NewShield)
{
	CurrentEnemyShield = FMath::Clamp<float>(NewShield, 0.0f, MaxEnemyShield);
}


void UEnemyCharacterStatComponent::OnRep_CurrentEnemyHp()
{
	OnHpChanged.Broadcast();
	if (CurrentEnemyHp <= 0.0f)
	{
		OnHpZero.Broadcast();
	}
}

void UEnemyCharacterStatComponent::OnRep_MaxEnemyHp()
{
	OnHpChanged.Broadcast();
}

void UEnemyCharacterStatComponent::OnRep_CurrentEnemyShield()
{
	OnShieldChanged.Broadcast();
	if (CurrentEnemyShield <= 0.0f)
	{
		OnShieldZero.Broadcast();
	}
}

void UEnemyCharacterStatComponent::OnRep_MaxEnemyShield()
{
	OnShieldChanged.Broadcast();
}
