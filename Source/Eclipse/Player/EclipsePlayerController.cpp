// Fill out your copyright notice in the Description page of Project Settings.


#include "EclipsePlayerController.h"

#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/Enemy/Boss.h"
#include "Eclipse/Game/EclipsePlayerState.h"
#include "Eclipse/UI/BossHPWidget.h"
#include "Kismet/GameplayStatics.h"

void AEclipsePlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());	
	if (::IsValid(PlayerCharacter))
	{
		InventoryManager = Cast<AEclipsePlayerState>(PlayerCharacter->GetPlayerState());
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

void AEclipsePlayerController::UpdateBossHpWidget(const float InCurrentHp, const float InMaxHp)
{
	if(::IsValid(BossHPWidget))
	{
		BossHPWidget->UpdateHPWidget(InCurrentHp, InMaxHp);
	}
}

void AEclipsePlayerController::UpdateBossShieldWidget(const float InCurrentShield, const float InMaxShield)
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

EDragOperationType AEclipsePlayerController::GetDragOperationType(const int32 DraggedIndex)
{
	if (DraggedIndex >= MinInventoryDragIndexRange && DraggedIndex <= MaxInventoryDragIndexRange)
	{
		return EDragOperationType::Inventory;
	}
	else if (DraggedIndex >= MinGearSlotDragIndexRange && DraggedIndex <= MaxGearSlotDragIndexRange)
	{
		return EDragOperationType::GearSlot;
	}
	else if (DraggedIndex >= MinGroundDragIndexRange && DraggedIndex <= MaxGroundDragIndexRange)
	{
		return EDragOperationType::Ground;
	}
	else if (DraggedIndex >= MinDeadBodyDragIndexRange && DraggedIndex <= MaxDeadBodyDragIndexRange)
	{
		return EDragOperationType::DeadBody;
	}

	return EDragOperationType::Invalid;
}

void AEclipsePlayerController::HandleDragAndDrop(const int32 DraggedIndex, const int32 DroppedIndex)
{
	// DraggedIndex 범위에 따른 DragOperationType을 지정
	EDragOperationType OperationType = GetDragOperationType(DraggedIndex);
	
	// OperationType에 따른 switch-case 처리
	switch (OperationType)
	{
	case EDragOperationType::Inventory:
		InventoryManager->DragFromInventory(PlayerCharacter, DraggedIndex, DroppedIndex);
		break;

	case EDragOperationType::GearSlot:
		InventoryManager->DragFromGearSlot(PlayerCharacter, DraggedIndex, DroppedIndex);
		break;

	case EDragOperationType::Ground:
		InventoryManager->DragFromGround(PlayerCharacter, PlayerInventoryStruct, DroppedIndex, IsAmmunition);
		break;

	case EDragOperationType::DeadBody:
		InventoryManager->DragFromDeadBody(PlayerCharacter, DraggedIndex, DroppedIndex);
		break;

	case EDragOperationType::Invalid:
	default:
		UE_LOG(LogTemp, Warning, TEXT("DraggedIndex (%d)는 유효하지 않은 범위입니다."), DraggedIndex);
		break;
	}
}



void AEclipsePlayerController::AddBossHpWidgetToViewport()
{
	TArray<class AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoss::StaticClass(), OutActors);
	for (const auto Bosses : OutActors)
	{
		if(const auto Boss = Cast<ABoss>(Bosses))
		{
			Boss->EnemyStat->OnHpChanged.AddUniqueDynamic(this, &AEclipsePlayerController::UpdateBossHpWidget);			
			Boss->EnemyStat->OnShieldChanged.AddUniqueDynamic(this, &AEclipsePlayerController::UpdateBossShieldWidget);
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


