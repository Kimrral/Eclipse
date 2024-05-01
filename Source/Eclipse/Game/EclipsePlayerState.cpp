// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Game/EclipsePlayerState.h"

#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


AEclipsePlayerState::AEclipsePlayerState()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NetUpdateFrequency = 5.f;
}

void AEclipsePlayerState::BeginPlay()
{
	Super::BeginPlay();

	IsAlreadyAccessed = false;
	PlayerInventoryStructs.Init(InventoryStructDefault, 30);
	PlayerInventoryStacks.Init(0, 30);
	PlayerGearSlotStructs.Init(InventoryStructDefault, 5);
}

void AEclipsePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	UE_LOG(LogTemp, Warning, TEXT("CopyProperties C++!"))
}

void AEclipsePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEclipsePlayerState, PlayerInventoryStructs)
	DOREPLIFETIME(AEclipsePlayerState, PlayerInventoryStacks)
	DOREPLIFETIME(AEclipsePlayerState, PlayerGearSlotStructs)
	DOREPLIFETIME(AEclipsePlayerState, DeadPlayerInventoryStructs)
	DOREPLIFETIME(AEclipsePlayerState, DeadPlayerInventoryStacks)
	DOREPLIFETIME(AEclipsePlayerState, DeadPlayerGearSlotStructs)
}

void AEclipsePlayerState::AddToInventory(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct)
{
	AddToInventoryServer(PlayerCharacterRef, PlayerInventoryStruct);
}

void AEclipsePlayerState::AddToInventoryServer_Implementation(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct)
{
	AddToInventoryMulticast(PlayerCharacterRef, PlayerInventoryStruct);
}

bool AEclipsePlayerState::AddToInventoryServer_Validate(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct)
{
	return true;
}

void AEclipsePlayerState::AddToInventoryMulticast_Implementation(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct)
{
	if (PlayerCharacterRef->IsLocallyControlled()) AddToInventoryWidget(PlayerCharacterRef, PlayerInventoryStruct);
	if (HasAuthority())
	{
		for (int i = 0; i < PlayerInventoryStructs.Num(); ++i)
		{
			if (PlayerInventoryStructs[i].Name == PlayerInventoryStruct.Name)
			{
				const int32 InventoryArrayIndex = i;
				PlayerInventoryStacks[InventoryArrayIndex]++;
				IsAlreadySet = true;
			}
		}
		if (IsAlreadySet)
		{
			IsAlreadySet = false;
		}
		else
		{
			for (int i = 0; i < PlayerInventoryStacks.Num(); ++i)
			{
				if (PlayerInventoryStacks[i] == 0)
				{
					const int32 InventoryArrayIndex = i;
					PlayerInventoryStructs[InventoryArrayIndex] = PlayerInventoryStruct;
					PlayerInventoryStacks[InventoryArrayIndex]++;
					return;
				}
			}
		}
	}
}

void AEclipsePlayerState::OnUseConsumableItem(APlayerCharacter* PlayerCharacterRef, const FString& ConsumableItemName, const float HealAmount)
{
	OnUseConsumableItemServer(PlayerCharacterRef, ConsumableItemName, HealAmount);
}


bool AEclipsePlayerState::OnUseConsumableItemServer_Validate(APlayerCharacter* PlayerCharacterRef, const FString& ConsumableItemName, const float HealAmount)
{
	return true;
}

void AEclipsePlayerState::OnUseConsumableItemServer_Implementation(APlayerCharacter* PlayerCharacterRef, const FString& ConsumableItemName, const float HealAmount)
{
	PlayerCharacterRef->Stat->HealHp(HealAmount);
	for (int i = 0; i < PlayerInventoryStructs.Num(); ++i)
	{
		if (PlayerInventoryStructs[i].Name == ConsumableItemName)
		{
			const int32 ConsumableItemArrayIndex = i;
			if (PlayerInventoryStacks[ConsumableItemArrayIndex] > 1)
			{
				PlayerInventoryStacks[ConsumableItemArrayIndex]--;
				return;
			}
			PlayerInventoryStructs[ConsumableItemArrayIndex] = InventoryStructDefault;
			PlayerInventoryStacks[ConsumableItemArrayIndex]--;
			return;
		}
	}
}

void AEclipsePlayerState::DragFromDeadBody(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	DragFromDeadBodyServer(PlayerCharacterRef, DragArrayIndex, DropArrayIndex);
}


void AEclipsePlayerState::DragFromDeadBodyServer_Implementation(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	DragFromDeadBodyMulticast(PlayerCharacterRef, DragArrayIndex, DropArrayIndex);
}

bool AEclipsePlayerState::DragFromDeadBodyServer_Validate(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	return true;
}

void AEclipsePlayerState::DragFromDeadBodyMulticast_Implementation(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	if (HasAuthority())
	{
		if (DropArrayIndex >= 81) return;
		if (DragArrayIndex >= 111)
		{
			PlayerInventoryStructs[DropArrayIndex] = DeadPlayerGearSlotStructs[DragArrayIndex - 111];
			PlayerInventoryStacks[DropArrayIndex] = 1;
			DeadPlayerGearSlotStructs[DragArrayIndex - 111] = InventoryStructDefault;
		}
		else
		{
			PlayerInventoryStructs[DropArrayIndex] = DeadPlayerInventoryStructs[DragArrayIndex - 81];
			PlayerInventoryStacks[DropArrayIndex] = DeadPlayerInventoryStacks[DragArrayIndex - 81];
			DeadPlayerInventoryStructs[DragArrayIndex - 81] = InventoryStructDefault;
			DeadPlayerInventoryStacks[DragArrayIndex - 81] = 0;
		}
	}
}


void AEclipsePlayerState::DragFromGearSlot(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	DragFromGearSlotServer(PlayerCharacterRef, DragArrayIndex, DropArrayIndex);
}

bool AEclipsePlayerState::DragFromGearSlotServer_Validate(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	return true;
}

void AEclipsePlayerState::DragFromGearSlotServer_Implementation(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	DragFromGearSlotMulticast(PlayerCharacterRef, DragArrayIndex, DropArrayIndex);
}

void AEclipsePlayerState::DragFromGearSlotMulticast_Implementation(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	if (HasAuthority())
	{
		if (DragArrayIndex == 38 && PlayerInventoryStacks[DropArrayIndex] == 0)
		{
			PlayerCharacterRef->EquipHelmetInventorySlot(false);
			PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - 38];
			PlayerInventoryStacks[DropArrayIndex]++;
			PlayerGearSlotStructs[DragArrayIndex - 38] = InventoryStructDefault;
		}
		else if (DragArrayIndex == 39 && PlayerInventoryStacks[DropArrayIndex] == 0)
		{
			PlayerCharacterRef->EquipGoggleInventorySlot(false);
			PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - 38];
			PlayerInventoryStacks[DropArrayIndex]++;
			PlayerGearSlotStructs[DragArrayIndex - 38] = InventoryStructDefault;
		}
		else if (DragArrayIndex == 40 && PlayerInventoryStacks[DropArrayIndex] == 0)
		{
			PlayerCharacterRef->EquipArmorInventorySlot(false);
			PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - 38];
			PlayerInventoryStacks[DropArrayIndex]++;
			PlayerGearSlotStructs[DragArrayIndex - 38] = InventoryStructDefault;
		}
		else if (DragArrayIndex == 41 && PlayerInventoryStacks[DropArrayIndex] == 0)
		{
			PlayerCharacterRef->EquipMaskInventorySlot(false);
			PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - 38];
			PlayerInventoryStacks[DropArrayIndex]++;
			PlayerGearSlotStructs[DragArrayIndex - 38] = InventoryStructDefault;
		}
		else if (DragArrayIndex == 42 && PlayerInventoryStacks[DropArrayIndex] == 0)
		{
			PlayerCharacterRef->EquipHeadsetInventorySlot(false);
			PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - 38];
			PlayerInventoryStacks[DropArrayIndex]++;
			PlayerGearSlotStructs[DragArrayIndex - 38] = InventoryStructDefault;
		}
	}
}

void AEclipsePlayerState::DragFromGround(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct, const int32 DropArrayIndex)
{
	DragFromGroundServer(PlayerCharacterRef, PlayerInventoryStruct, DropArrayIndex);
}


void AEclipsePlayerState::DragFromGroundServer_Implementation(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct, const int32 DropArrayIndex)
{
	DragFromGroundMulticast(PlayerCharacterRef, PlayerInventoryStruct, DropArrayIndex);
}

bool AEclipsePlayerState::DragFromGroundServer_Validate(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct, const int32 DropArrayIndex)
{
	return true;
}


void AEclipsePlayerState::DragFromGroundMulticast_Implementation(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct, const int32 DropArrayIndex)
{
	if (HasAuthority())
	{
		if (PlayerInventoryStacks[DropArrayIndex] == 0)
		{
			PlayerInventoryStructs[DropArrayIndex] = PlayerInventoryStruct;
			PlayerInventoryStacks[DropArrayIndex]++;
			GroundDetectAndDestroy(PlayerCharacterRef->GetActorLocation(), PlayerInventoryStruct.Name);
		}
		else
		{
			AddToInventory(PlayerCharacterRef, PlayerInventoryStruct);
			GroundDetectAndDestroy(PlayerCharacterRef->GetActorLocation(), PlayerInventoryStruct.Name);
		}
	}
	if (PlayerCharacterRef->IsLocallyControlled()) UGameplayStatics::PlaySound2D(GetWorld(), PlayerCharacterRef->PickUpSound);
	PlayerCharacterRef->PlayAnimMontage(PlayerCharacterRef->UpperOnlyMontage, 1, FName("WeaponEquip"));
}

void AEclipsePlayerState::DragFromInventory(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	DragFromInventoryServer(PlayerCharacterRef, DragArrayIndex, DropArrayIndex);
}

void AEclipsePlayerState::DragFromInventoryServer_Implementation(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	DragFromInventoryMulticast(PlayerCharacterRef, DragArrayIndex, DropArrayIndex);
}

bool AEclipsePlayerState::DragFromInventoryServer_Validate(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	return true;
}

void AEclipsePlayerState::DragFromInventoryMulticast_Implementation(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	if (HasAuthority())
	{
		if (DropArrayIndex == 38)
		{
			if (PlayerInventoryStructs[DragArrayIndex].ItemType == FString("Helmet"))
			{
				PlayerCharacterRef->EquipHelmetInventorySlot(true);
				if (PlayerInventoryStacks[DragArrayIndex] > 1)
				{
					PlayerGearSlotStructs[0] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
				}
				else
				{
					PlayerGearSlotStructs[0] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
					PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
				}
			}
			return;
		}
		if (DropArrayIndex == 39)
		{
			if (PlayerInventoryStructs[DragArrayIndex].ItemType == FString("Goggle"))
			{
				PlayerCharacterRef->EquipGoggleInventorySlot(true);
				if (PlayerInventoryStacks[DragArrayIndex] > 1)
				{
					PlayerGearSlotStructs[1] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
				}
				else
				{
					PlayerGearSlotStructs[1] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
					PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
				}
			}
			return;
		}
		if (DropArrayIndex == 40)
		{
			if (PlayerInventoryStructs[DragArrayIndex].ItemType == FString("Armor"))
			{
				PlayerCharacterRef->EquipArmorInventorySlot(true);
				if (PlayerInventoryStacks[DragArrayIndex] > 1)
				{
					PlayerGearSlotStructs[2] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
				}
				else
				{
					PlayerGearSlotStructs[2] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
					PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
				}
			}
			return;
		}
		if (DropArrayIndex == 41)
		{
			if (PlayerInventoryStructs[DragArrayIndex].ItemType == FString("Mask"))
			{
				PlayerCharacterRef->EquipMaskInventorySlot(true);
				if (PlayerInventoryStacks[DragArrayIndex] > 1)
				{
					PlayerGearSlotStructs[3] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
				}
				else
				{
					PlayerGearSlotStructs[3] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
					PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
				}
			}
			return;
		}
		if (DropArrayIndex == 42)
		{
			if (PlayerInventoryStructs[DragArrayIndex].ItemType == FString("Headset"))
			{
				PlayerCharacterRef->EquipHeadsetInventorySlot(true);
				if (PlayerInventoryStacks[DragArrayIndex] > 1)
				{
					PlayerGearSlotStructs[4] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
				}
				else
				{
					PlayerGearSlotStructs[4] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
					PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
				}
			}
			return;
		}
		if (DropArrayIndex == 45)
		{
			if (PlayerInventoryStacks[DragArrayIndex] > 1)
			{
				FActorSpawnParameters Param;
				Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				GetWorld()->SpawnActor<AActor>(PlayerInventoryStructs[DragArrayIndex].ActorReference, PlayerCharacterRef->GetMesh()->GetSocketTransform(FName("ItemDrop")), Param);
				PlayerInventoryStacks[DragArrayIndex]--;
			}
			else
			{
				FActorSpawnParameters Param;
				Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				GetWorld()->SpawnActor<AActor>(PlayerInventoryStructs[DragArrayIndex].ActorReference, PlayerCharacterRef->GetMesh()->GetSocketTransform(FName("ItemDrop")), Param);
				PlayerInventoryStacks[DragArrayIndex]--;
				PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
			}
			return;
		}
		// Move Item
		if (PlayerInventoryStacks[DropArrayIndex] == 0)
		{
			PlayerInventoryStructs[DropArrayIndex] = PlayerInventoryStructs[DragArrayIndex];
			PlayerInventoryStacks[DropArrayIndex] = PlayerInventoryStacks[DragArrayIndex];
			PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
			PlayerInventoryStacks[DragArrayIndex] = 0;
			return;
		}
		// Merge Item
		if (PlayerInventoryStructs[DragArrayIndex].Name == PlayerInventoryStructs[DropArrayIndex].Name)
		{
			PlayerInventoryStacks[DropArrayIndex] += PlayerInventoryStacks[DragArrayIndex];
			PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
			return;
		}
		//Swap Item
		if (PlayerInventoryStructs[DragArrayIndex].Name != PlayerInventoryStructs[DropArrayIndex].Name)
		{
			InventoryDropStructCache = PlayerInventoryStructs[DropArrayIndex];
			InventoryDropStackCache = PlayerInventoryStacks[DropArrayIndex];
			PlayerInventoryStructs[DropArrayIndex] = PlayerInventoryStructs[DragArrayIndex];
			PlayerInventoryStacks[DropArrayIndex] = PlayerInventoryStacks[DragArrayIndex];
			PlayerInventoryStructs[DragArrayIndex] = InventoryDropStructCache;
			PlayerInventoryStacks[DragArrayIndex] = InventoryDropStackCache;
		}
	}
}


void AEclipsePlayerState::DeadBodyWidgetSettings(ADeadPlayerContainer* DeadPlayerContainer, APlayerCharacter* InstigatorPlayerRef)
{
	DeadBodyWidgetSettingsServer(DeadPlayerContainer, InstigatorPlayerRef);
}

void AEclipsePlayerState::DeadBodyWidgetSettingsServer_Implementation(ADeadPlayerContainer* DeadPlayerContainer, APlayerCharacter* InstigatorPlayerRef)
{
	DeadBodyWidgetSettingsMulticast(DeadPlayerContainer, InstigatorPlayerRef);
}

bool AEclipsePlayerState::DeadBodyWidgetSettingsServer_Validate(ADeadPlayerContainer* DeadPlayerContainer, APlayerCharacter* InstigatorPlayerRef)
{
	return true;
}

void AEclipsePlayerState::DeadBodyWidgetSettingsMulticast_Implementation(ADeadPlayerContainer* DeadPlayerContainer, APlayerCharacter* InstigatorPlayerRef)
{
	if (!IsAlreadyAccessed)
	{
		DeadPlayerInventoryStructs = DeadPlayerContainer->DeadPlayerInventoryStructArray;
		DeadPlayerInventoryStacks = DeadPlayerContainer->DeadPlayerInventoryStackArray;
		DeadPlayerGearSlotStructs = DeadPlayerContainer->DeadPlayerGearSlotArray;
		IsAlreadyAccessed = true;
	}
	if (InstigatorPlayerRef->IsLocallyControlled())
	{
		DeadBodySettingsOnWidgetClass(InstigatorPlayerRef, DeadPlayerInventoryStructs, DeadPlayerInventoryStacks, DeadPlayerGearSlotStructs);
	}
}


void AEclipsePlayerState::RemoveSoldInventoryIndex(APlayerCharacter* PlayerCharacterRef, const TArray<int32>& SoldInventoryIndexArray, const int32 SoldRoubleAmount)
{
	RemoveSoldInventoryIndexServer(PlayerCharacterRef, SoldInventoryIndexArray, SoldRoubleAmount);
}

void AEclipsePlayerState::RemoveSoldInventoryIndexServer_Implementation(APlayerCharacter* PlayerCharacterRef, const TArray<int32>& SoldInventoryIndexArray, const int32 SoldRoubleAmount)
{
	for (int i = 0; i < SoldInventoryIndexArray.Num(); ++i)
	{
		PlayerInventoryStructs[SoldInventoryIndexArray[i]] = InventoryStructDefault;
		PlayerInventoryStacks[SoldInventoryIndexArray[i]] = 0;
	}
	if (PlayerCharacterRef)
	{
		PlayerCharacterRef->Stat->AddRouble(SoldRoubleAmount);
	}
}

bool AEclipsePlayerState::RemoveSoldInventoryIndexServer_Validate(APlayerCharacter* PlayerCharacterRef, const TArray<int32>& SoldInventoryIndexArray, const int32 SoldRoubleAmount)
{
	return true;
}

void AEclipsePlayerState::ModifyRouble(APlayerCharacter* PlayerCharacterRef, const float RoubleAmount)
{
	ModifyRoubleServer(PlayerCharacterRef, RoubleAmount);
}

void AEclipsePlayerState::ModifyRoubleServer_Implementation(APlayerCharacter* PlayerCharacterRef, const float RoubleAmount)
{
	if(PlayerCharacterRef)
	{
		PlayerCharacterRef->Stat->AddRouble(RoubleAmount);
	}
}

bool AEclipsePlayerState::ModifyRoubleServer_Validate(APlayerCharacter* PlayerCharacterRef, const float RoubleAmount)
{
	return true;
}
