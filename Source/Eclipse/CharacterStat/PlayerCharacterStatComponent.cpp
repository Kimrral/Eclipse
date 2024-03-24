// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterStatComponent.h"

#include "Eclipse/Player/EclipsePlayerController.h"
#include "Eclipse/UI/CrosshairWidget.h"
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


float UPlayerCharacterStatComponent::ApplyDamage(float InDamage, AActor* DamageCauser)
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
}

void UPlayerCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);
}


void UPlayerCharacterStatComponent::OnRep_CurrentHp()
{
	OnHpChanged.Broadcast();
	if (CurrentHp <= 0.0f)
	{
		OnHpZero.Broadcast();
	}
}

void UPlayerCharacterStatComponent::OnRep_MaxHp()
{
	OnHpChanged.Broadcast();
}
