// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipsePlayerController.h"

#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/Enemy/Boss.h"
#include "Eclipse/UI/BossHPWidget.h"
#include "Kismet/GameplayStatics.h"

void AEclipsePlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	if (::IsValid(PlayerCharacter))
	{
		PlayerCharacter->Stat->OnHpChanged.AddUObject(this, &AEclipsePlayerController::UpdateTabWidget);
		PlayerCharacter->Stat->OnHpZero.AddUObject(this, &AEclipsePlayerController::PlayerDeath);
	}

	BossHPWidget = CreateWidget<UBossHPWidget>(GetWorld(), BossHPWidgetFactory);	
}

void AEclipsePlayerController::UpdateTabWidget() const
{
	if (PlayerCharacter)
	{
		PlayerCharacter->UpdateTabWidgetHP();
	}
}

void AEclipsePlayerController::UpdateBossHpWidget(const float InCurrentHp, const float InMaxHp) const
{
	if(::IsValid(BossHPWidget))
	{
		BossHPWidget->UpdateHPWidget(InCurrentHp, InMaxHp);
	}
}

void AEclipsePlayerController::UpdateBossShieldWidget(const float InCurrentShield, const float InMaxShield) const
{
	if(::IsValid(BossHPWidget))
	{
		BossHPWidget->UpdateShieldWidget(InCurrentShield, InMaxShield);
	}
}


void AEclipsePlayerController::PlayerDeath() const
{
	if (PlayerCharacter)
	{
		PlayerCharacter->PlayerDeathRPCServer();
	}
}

void AEclipsePlayerController::AddBossHpWidgetToViewport() const
{
	TArray<class AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoss::StaticClass(), OutActors);
	for (const auto Bosses : OutActors)
	{
		if(const auto Boss = Cast<ABoss>(Bosses))
		{
			Boss->EnemyStat->OnHpChanged.AddUObject(this, &AEclipsePlayerController::UpdateBossHpWidget);			
			Boss->EnemyStat->OnShieldChanged.AddUObject(this, &AEclipsePlayerController::UpdateBossShieldWidget);
			UpdateBossHpWidget(Boss->EnemyStat->GetCurrentHp(), Boss->EnemyStat->GetMaxHp());
			UpdateBossShieldWidget(Boss->EnemyStat->GetCurrentShield(), Boss->EnemyStat->GetMaxShield());
			break;
		}
	}
	if(::IsValid(BossHPWidget) && !BossHPWidget->IsInViewport())
	{
		BossHPWidget->AddToViewport();
	}
}

void AEclipsePlayerController::RemoveBossHpWidgetFromViewport() const
{
	TArray<class AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoss::StaticClass(), OutActors);
	for (const auto Bosses : OutActors)
	{
		if(const auto Boss = Cast<ABoss>(Bosses))
		{
			Boss->EnemyStat->OnHpChanged.Clear();			
			Boss->EnemyStat->OnShieldChanged.Clear();
			break;
		}
	}
	if(::IsValid(BossHPWidget) && BossHPWidget->IsInViewport())
	{
		BossHPWidget->RemoveFromParent();
	}
}


