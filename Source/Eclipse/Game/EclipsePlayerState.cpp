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

	PlayerInventoryStructs.Init(InventoryStructDefault, 30);
	PlayerInventoryStacks.Init(0, 30);
	PlayerGearSlotStructs.Init(InventoryStructDefault, 5);
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

void AEclipsePlayerState::AddToInventoryMulticast_Implementation(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct)
{
	if (PlayerCharacterRef)
	{
		if (PlayerCharacterRef->IsLocallyControlled())
		{
			AddToInventoryWidget(PlayerCharacterRef, PlayerInventoryStruct);
		}
	}
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

void AEclipsePlayerState::OnUseConsumableItemServer_Implementation(APlayerCharacter* PlayerCharacterRef, const FString& ConsumableItemName, const float HealAmount)
{
	if (PlayerCharacterRef)
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
}

void AEclipsePlayerState::DragFromDeadBody(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	DragFromDeadBodyServer(PlayerCharacterRef, DragArrayIndex, DropArrayIndex);
}


void AEclipsePlayerState::DragFromDeadBodyServer_Implementation(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	if (DropArrayIndex >= 81) return;
	if (DragArrayIndex >= 111)
	{
		if (PlayerInventoryStructs.IsValidIndex(DropArrayIndex) && PlayerInventoryStacks.IsValidIndex(DropArrayIndex) && DeadPlayerGearSlotStructs.IsValidIndex(DragArrayIndex - 111))
		{
			PlayerInventoryStructs[DropArrayIndex] = DeadPlayerGearSlotStructs[DragArrayIndex - 111];
			PlayerInventoryStacks[DropArrayIndex] = 1;
			DeadPlayerGearSlotStructs[DragArrayIndex - 111] = InventoryStructDefault;
		}
	}
	else
	{
		if (PlayerInventoryStructs.IsValidIndex(DropArrayIndex) && PlayerInventoryStacks.IsValidIndex(DropArrayIndex) && DeadPlayerInventoryStructs.IsValidIndex(DragArrayIndex - 81))
		{
			PlayerInventoryStructs[DropArrayIndex] = DeadPlayerInventoryStructs[DragArrayIndex - 81];
			PlayerInventoryStacks[DropArrayIndex] = DeadPlayerInventoryStacks[DragArrayIndex - 81];
			DeadPlayerInventoryStructs[DragArrayIndex - 81] = InventoryStructDefault;
			DeadPlayerInventoryStacks[DragArrayIndex - 81] = 0;
		}
	}
	if (DeadPlayerContainerRef)
	{
		DeadPlayerContainerRef->DeadPlayerInventoryStructArray = DeadPlayerInventoryStructs;
		DeadPlayerContainerRef->DeadPlayerInventoryStackArray = DeadPlayerInventoryStacks;
		DeadPlayerContainerRef->DeadPlayerGearSlotArray = DeadPlayerGearSlotStructs;
	}
}

void AEclipsePlayerState::DragFromGearSlot(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	DragFromGearSlotServer(PlayerCharacterRef, DragArrayIndex, DropArrayIndex);
}

void AEclipsePlayerState::DragFromGearSlotServer_Implementation(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	if (PlayerCharacterRef)
	{
		if (PlayerInventoryStructs.IsValidIndex(DropArrayIndex) && PlayerInventoryStacks.IsValidIndex(DropArrayIndex) && PlayerGearSlotStructs.IsValidIndex(DragArrayIndex - 38))
		{
			if (DragArrayIndex == 38 && PlayerInventoryStacks[DropArrayIndex] == 0)
			{
				PlayerCharacterRef->EquipHelmetInventorySlot(false, 1);
				PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - 38];
				PlayerInventoryStacks[DropArrayIndex]++;
				PlayerGearSlotStructs[DragArrayIndex - 38] = InventoryStructDefault;
			}
			else if (DragArrayIndex == 39 && PlayerInventoryStacks[DropArrayIndex] == 0)
			{
				PlayerCharacterRef->EquipGoggleInventorySlot(false, 1);
				PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - 38];
				PlayerInventoryStacks[DropArrayIndex]++;
				PlayerGearSlotStructs[DragArrayIndex - 38] = InventoryStructDefault;
			}
			else if (DragArrayIndex == 40 && PlayerInventoryStacks[DropArrayIndex] == 0)
			{
				PlayerCharacterRef->EquipArmorInventorySlot(false, PlayerGearSlotStructs[DragArrayIndex - 38].Stat);
				PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - 38];
				PlayerInventoryStacks[DropArrayIndex]++;
				PlayerGearSlotStructs[DragArrayIndex - 38] = InventoryStructDefault;
			}
			else if (DragArrayIndex == 41 && PlayerInventoryStacks[DropArrayIndex] == 0)
			{
				PlayerCharacterRef->EquipMaskInventorySlot(false, 1);
				PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - 38];
				PlayerInventoryStacks[DropArrayIndex]++;
				PlayerGearSlotStructs[DragArrayIndex - 38] = InventoryStructDefault;
			}
			else if (DragArrayIndex == 42 && PlayerInventoryStacks[DropArrayIndex] == 0)
			{
				PlayerCharacterRef->EquipHeadsetInventorySlot(false, 1);
				PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - 38];
				PlayerInventoryStacks[DropArrayIndex]++;
				PlayerGearSlotStructs[DragArrayIndex - 38] = InventoryStructDefault;
			}
		}
	}
}

// 그라운드 아이템 리스트 드래그
void AEclipsePlayerState::DragFromGround(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct, const int32 DropArrayIndex, const bool IsAmmunition)
{
	DragFromGroundServer(PlayerCharacterRef, PlayerInventoryStruct, DropArrayIndex, IsAmmunition);
}


void AEclipsePlayerState::DragFromGroundServer_Implementation(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct, const int32 DropArrayIndex, const bool IsAmmunition)
{
	if (PlayerCharacterRef)
	{
		if (IsAmmunition)
		{
			// 탄약 아이템일 경우, 해당 아이템 구조체 데이터 타입에 따라 탄약 보충
			PlayerCharacterRef->AddAmmunitionByInputString(PlayerInventoryStruct.Name);
			// 드래그 대상 아이템 액터 파괴
			GroundDetectAndDestroy(PlayerCharacterRef->GetActorLocation(), PlayerInventoryStruct.Name);
		}
		else
		{
			if (PlayerInventoryStructs.IsValidIndex(DropArrayIndex) && PlayerInventoryStacks.IsValidIndex(DropArrayIndex))
			{
				// 드랍한 인벤토리 인덱스에 아이템이 없다면
				if (PlayerInventoryStacks[DropArrayIndex] == 0)
				{
					// 매개변수 구조체로 해당 인벤토리 인덱스 채우기
					PlayerInventoryStructs[DropArrayIndex] = PlayerInventoryStruct;
					PlayerInventoryStacks[DropArrayIndex]++;
					GroundDetectAndDestroy(PlayerCharacterRef->GetActorLocation(), PlayerInventoryStruct.Name);
				}
				// 드랍한 인벤토리 인덱스에 아이템이 있다면
				else
				{
					// 인벤토리 아이템 자동 추가 함수 호출
					AddToInventory(PlayerCharacterRef, PlayerInventoryStruct);
					GroundDetectAndDestroy(PlayerCharacterRef->GetActorLocation(), PlayerInventoryStruct.Name);
				}
			}
		}
	}

	// 멀티캐스트 RPC 호출
	DragFromGroundMulticast(PlayerCharacterRef, PlayerInventoryStruct, DropArrayIndex, IsAmmunition);
}

void AEclipsePlayerState::DragFromGroundMulticast_Implementation(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct, const int32 DropArrayIndex, const bool IsAmmunition)
{
	if (PlayerCharacterRef)
	{
		if (PlayerCharacterRef->IsLocallyControlled())
		{
			// 로컬 클라이언트 픽업 2D 사운드 재생
			UGameplayStatics::PlaySound2D(GetWorld(), PlayerCharacterRef->PickUpSound);
		}
		if (!PlayerCharacterRef->HasAuthority())
		{
			// 모든 클라이언트에서 픽업 애니메이션 몽타주 재생
			PlayerCharacterRef->PlayAnimMontage(PlayerCharacterRef->UpperOnlyMontage, 1, FName("WeaponEquip"));
		}
	}
}

void AEclipsePlayerState::DragFromInventory(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	DragFromInventoryServer(PlayerCharacterRef, DragArrayIndex, DropArrayIndex);
}

void AEclipsePlayerState::DragFromInventoryServer_Implementation(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	if (PlayerCharacterRef)
	{
		if (DropArrayIndex == 38)
		{
			if (PlayerInventoryStructs[DragArrayIndex].ItemType == FString("Helmet"))
			{
				PlayerCharacterRef->EquipHelmetInventorySlot(true, PlayerInventoryStructs[DragArrayIndex].Stat);
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
				PlayerCharacterRef->EquipGoggleInventorySlot(true, PlayerInventoryStructs[DragArrayIndex].Stat);
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
				PlayerCharacterRef->EquipArmorInventorySlot(true, PlayerInventoryStructs[DragArrayIndex].Stat);
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
				PlayerCharacterRef->EquipMaskInventorySlot(true, PlayerInventoryStructs[DragArrayIndex].Stat);
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
				PlayerCharacterRef->EquipHeadsetInventorySlot(true, PlayerInventoryStructs[DragArrayIndex].Stat);
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
	if (DeadPlayerContainer)
	{
		DeadPlayerContainerRef = DeadPlayerContainer;
		DeadPlayerInventoryStructs = DeadPlayerContainer->DeadPlayerInventoryStructArray;
		DeadPlayerInventoryStacks = DeadPlayerContainer->DeadPlayerInventoryStackArray;
		DeadPlayerGearSlotStructs = DeadPlayerContainer->DeadPlayerGearSlotArray;
	}

	DeadBodyWidgetSettingsMulticast(DeadPlayerContainer, InstigatorPlayerRef);
}

void AEclipsePlayerState::DeadBodyWidgetSettingsMulticast_Implementation(ADeadPlayerContainer* DeadPlayerContainer, APlayerCharacter* InstigatorPlayerRef)
{
	if (InstigatorPlayerRef && InstigatorPlayerRef->IsLocallyControlled())
	{
		DeadBodySettingsOnWidgetClass(InstigatorPlayerRef,
		                              DeadPlayerContainer->DeadPlayerInventoryStructArray,
		                              DeadPlayerContainer->DeadPlayerInventoryStackArray,
		                              DeadPlayerContainer->DeadPlayerGearSlotArray);
	}
}

// 판매 완료한 인벤토리 아이템 제거 함수
void AEclipsePlayerState::RemoveSoldInventoryIndex(APlayerCharacter* PlayerCharacterRef, const TArray<int32>& SoldInventoryIndexArray, const int32 SoldRoubleAmount)
{
	RemoveSoldInventoryIndexServer(PlayerCharacterRef, SoldInventoryIndexArray, SoldRoubleAmount);
}

void AEclipsePlayerState::RemoveSoldInventoryIndexServer_Implementation(APlayerCharacter* PlayerCharacterRef, const TArray<int32>& SoldInventoryIndexArray, const int32 SoldRoubleAmount)
{
	// 판매 완료한 인벤토리 아이템 슬롯 인덱스 초기화
	for (int i = 0; i < SoldInventoryIndexArray.Num(); ++i)
	{
		PlayerInventoryStructs[SoldInventoryIndexArray[i]] = InventoryStructDefault;
		PlayerInventoryStacks[SoldInventoryIndexArray[i]] = 0;
	}
	// 판매한 아이템 가치만큼 스탯 컴포넌트의 보유 자산 증가 
	if (PlayerCharacterRef)
	{
		PlayerCharacterRef->Stat->AddRouble(SoldRoubleAmount);
	}
}

void AEclipsePlayerState::ModifyRouble(APlayerCharacter* PlayerCharacterRef, const float RoubleAmount)
{
	ModifyRoubleServer(PlayerCharacterRef, RoubleAmount);
}

void AEclipsePlayerState::ModifyRoubleServer_Implementation(APlayerCharacter* PlayerCharacterRef, const float RoubleAmount)
{
	if (PlayerCharacterRef)
	{
		PlayerCharacterRef->Stat->AddRouble(RoubleAmount);
	}
}

void AEclipsePlayerState::ResetPlayerInventoryData()
{
	PlayerInventoryStructs.Init(InventoryStructDefault, 30);
	PlayerInventoryStacks.Init(0, 30);
	PlayerGearSlotStructs.Init(InventoryStructDefault, 5);
}

void AEclipsePlayerState::ApplyGearInventoryEquipState(APlayerCharacter* PlayerCharacterRef)
{

}
