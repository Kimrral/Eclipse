// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStatController.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/Enemy/Boss.h"
#include "Eclipse/UI/BossHPWidget.h"

// Sets default values for this component's properties
UEnemyStatController::UEnemyStatController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UEnemyStatController::BeginPlay()
{
	Super::BeginPlay();

	BossHPWidget = CreateWidget<UBossHPWidget>(GetWorld(), BossHPWidgetFactory);
}


// Called every frame
void UEnemyStatController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UEnemyStatController::AddBossHpWidgetToViewport()
{
	TArray<class AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoss::StaticClass(), OutActors);
	for (const auto Bosses : OutActors)
	{
		if (const auto Boss = Cast<ABoss>(Bosses))
		{
			Boss->EnemyStat->OnHpChanged.AddUniqueDynamic(this, &UEnemyStatController::UpdateBossHpWidget);
			Boss->EnemyStat->OnShieldChanged.AddUniqueDynamic(this, &UEnemyStatController::UpdateBossShieldWidget);
			UpdateBossHpWidget(Boss->EnemyStat->GetCurrentHp(), Boss->EnemyStat->GetMaxHp());
			UpdateBossShieldWidget(Boss->EnemyStat->GetCurrentShield(), Boss->EnemyStat->GetMaxShield());
			break;
		}
	}
	if (::IsValid(BossHPWidget) && !BossHPWidget->IsInViewport())
	{
		BossHPWidget->AddToViewport();
	}
}

void UEnemyStatController::RemoveBossHpWidgetFromViewport() const
{
	TArray<class AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoss::StaticClass(), OutActors);
	for (const auto Bosses : OutActors)
	{
		if (const auto Boss = Cast<ABoss>(Bosses))
		{
			Boss->EnemyStat->OnHpChanged.Clear();
			Boss->EnemyStat->OnShieldChanged.Clear();
			break;
		}
	}
	if (::IsValid(BossHPWidget) && BossHPWidget->IsInViewport())
	{
		BossHPWidget->RemoveFromParent();
	}
}


void UEnemyStatController::UpdateBossHpWidget(const float InCurrentHp, const float InMaxHp)
{
	if (::IsValid(BossHPWidget))
	{
		BossHPWidget->UpdateHPWidget(InCurrentHp, InMaxHp);
	}
}

void UEnemyStatController::UpdateBossShieldWidget(const float InCurrentShield, const float InMaxShield)
{
	if (::IsValid(BossHPWidget))
	{
		BossHPWidget->UpdateShieldWidget(InCurrentShield, InMaxShield);
	}
}
