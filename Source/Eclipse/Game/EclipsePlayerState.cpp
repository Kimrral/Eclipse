// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Game/EclipsePlayerState.h"

#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/UI/TabWidget.h"
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
	// 클라이언트에서 아이템 습득 시각적 효과를 우선 실행
	if (IsValid(PlayerCharacterRef) && PlayerCharacterRef->IsLocallyControlled())
	{
		// 로컬 인벤토리 로직 실행, 클라이언트 UI 업데이트
		PlayerCharacterRef->tabWidgetUI->AddToInventoryLocal(PlayerCharacterRef, PlayerInventoryStruct);
		// 클라이언트 측 시각적 효과 출력
		AddToInventoryWidgetClass(PlayerCharacterRef, PlayerInventoryStruct);
		// 대상 아이템을 즉시 로컬에서 숨김 처리
		HidePickedUpItem(PlayerCharacterRef, PlayerInventoryStruct);
	}

	// 서버에 인벤토리 로직 요청
	AddToInventoryServer(PlayerCharacterRef, PlayerInventoryStruct);
}


void AEclipsePlayerState::AddToInventoryServer_Implementation(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct)
{
	// 서버에서 아이템 습득 처리 로직
	if (HasAuthority())
	{
		// 현재 인벤토리 상태를 캐싱
		const TArray<FPlayerInventoryStruct> CachedPlayerInventoryStructs = PlayerInventoryStructs;
		const TArray<int32> CachedPlayerInventoryStacks = PlayerInventoryStacks;

		// 기존에 같은 이름의 아이템이 있는지 확인
		bool bIsAlreadySet = false;

		for (int i = 0; i < PlayerInventoryStructs.Num(); ++i)
		{
			if (PlayerInventoryStructs[i].Name == PlayerInventoryStruct.Name)
			{
				PlayerInventoryStacks[i]++; // 기존 아이템 스택 증가
				bIsAlreadySet = true;
				break;
			}
		}

		// 중복 아이템이 없다면
		if (!bIsAlreadySet)
		{
			// 빈 슬롯을 검색하고 새로운 아이템 추가
			for (int i = 0; i < PlayerInventoryStacks.Num(); ++i)
			{
				if (PlayerInventoryStacks[i] == 0)
				{
					PlayerInventoryStructs[i] = PlayerInventoryStruct;
					PlayerInventoryStacks[i]++;
					break;
				}
			}
		}

		// 처리 이후 클라이언트와 서버의 인벤토리 데이터 일치 여부 검증
		// 대상 플레이어의 클라이언트 위젯 클래스 데이터와 비교
		if (IsValid(PlayerCharacterRef->tabWidgetUI))
		{
			bool bIsInventorySynced = true;

			// 클라이언트의 인벤토리 데이터 가져오기
			TArray<FPlayerInventoryStruct> ClientInventoryStructs = PlayerCharacterRef->tabWidgetUI->GetInventoryData();
			TArray<int32> ClientInventoryStacks = PlayerCharacterRef->tabWidgetUI->GetInventoryStacks();

			// 서버와 클라이언트의 인벤토리 데이터를 비교
			for (int32 i = 0; i < PlayerInventoryStructs.Num(); i++)
			{
				if (PlayerInventoryStructs[i].Name != ClientInventoryStructs[i].Name ||
					PlayerInventoryStacks[i] != ClientInventoryStacks[i])
				{
					bIsInventorySynced = false;
					break;
				}
			}

			// 데이터 검증 실패 시
			if (!bIsInventorySynced)
			{
				// 서버의 인벤토리 데이터를 롤백
				PlayerInventoryStructs = CachedPlayerInventoryStructs;
				PlayerInventoryStacks = CachedPlayerInventoryStacks;

				// 서버의 인벤토리 데이터로 재출력 요청
				ServerSyncInventory(PlayerCharacterRef);
				return;
			}
			// 데이터 검증 성공 시
			else
			{
				// 확정 로직 실행
				AddToInventoryMulticast(PlayerCharacterRef, PlayerInventoryStruct);
			}
		}
	}
}

// 서버 검증 성공 시 실행되는 확정 로직
void AEclipsePlayerState::AddToInventoryMulticast_Implementation(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct)
{
	if (IsValid(PlayerCharacterRef) && !PlayerCharacterRef->IsLocallyControlled())
	{
		// 시뮬레이트 프록시에서 아이템 습득 시각적 효과 실행     
		AddToInventoryWidget(PlayerCharacterRef, PlayerInventoryStruct);
	}

	if (HasAuthority())
	{
		// 서버에서 최종적으로 아이템을 파괴
		DestroyPickedUpItem(PlayerCharacterRef, PlayerInventoryStruct);
	}
}

void AEclipsePlayerState::DestroyPickedUpItem(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct) const
{
	// 서버에서 최종적으로 아이템을 파괴
	// 아이템 액터를 파괴하거나 메모리에서 제거
	if (IsValid(PlayerCharacterRef) && HasAuthority())
	{
		if (APickableActor* PickedUpItem = PlayerCharacterRef->FindItemByName(PlayerInventoryStruct.Name))
		{
			if (PickedUpItem)
			{
				PickedUpItem->Destroy(); // 아이템 액터 파괴
			}
		}
	}
}

void AEclipsePlayerState::AddToInventoryWidgetClass(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct) const
{
	// 클라이언트에서 아이템 습득 시각적 효과 실행
	if (IsValid(PlayerCharacterRef))
	{
		// 파티클 효과 재생
		if (const UWorld* World = GetWorld())
		{
			// 아이템 습득 몽타주 재생
			PlayerCharacterRef->PlayAnimMontage(PlayerCharacterRef->UpperOnlyMontage, 1, FName("WeaponEquip"));
			// 사운드 재생
			UGameplayStatics::PlaySoundAtLocation(World, PlayerCharacterRef->PickUpSound, PlayerCharacterRef->GetActorLocation());
		}
	}
}



void AEclipsePlayerState::HidePickedUpItem(APlayerCharacter* PlayerCharacterRef, const FPlayerInventoryStruct& PlayerInventoryStruct)
{
	// 로컬에서 아이템을 즉시 숨김 처리
	// 아이템 메쉬를 숨기거나 비활성화
	if (IsValid(PlayerCharacterRef))
	{
		if (APickableActor* PickedUpItem = PlayerCharacterRef->FindItemByName(PlayerInventoryStruct.Name))
		{
			PickedUpItem->SetActorHiddenInGame(true); // 아이템 메쉬 숨기기
			PickedUpItem->SetActorEnableCollision(false); // 아이템 충돌 비활성화
		}
	}
}

// 서버 검증 실패 시 실행되는 동기화 로직
void AEclipsePlayerState::ServerSyncInventory(APlayerCharacter* PlayerCharacterRef) const
{
	// // 클라이언트의 인벤토리 데이터를 서버의 데이터와 동기화
	// if (IsValid(PlayerCharacterRef))
	// {
	// 	if (UTabWidget* InventoryUI = PlayerCharacterRef->tabWidgetUI())
	// 	{
	// 		InventoryUI->SetInventoryData(PlayerInventoryStructs, PlayerInventoryStacks); // 서버 데이터를 클라이언트로 동기화
	// 	}
	// }
}

void AEclipsePlayerState::OnUseConsumableItem(APlayerCharacter* PlayerCharacterRef, const FString& ConsumableItemName, const float HealAmount)
{
	// 소모품 사용 함수 호출 - 서버 측 함수 호출
	OnUseConsumableItemServer(PlayerCharacterRef, ConsumableItemName, HealAmount);
}

void AEclipsePlayerState::OnUseConsumableItemServer_Implementation(APlayerCharacter* PlayerCharacterRef, const FString& ConsumableItemName, const float HealAmount)
{
	// 플레이어 캐릭터가 유효한지 확인
	if (PlayerCharacterRef)
	{
		// 플레이어 캐릭터의 체력을 회복
		PlayerCharacterRef->Stat->HealHp(HealAmount);
		// 플레이어 인벤토리에서 소모품을 찾아서 사용 처리
		for (int i = 0; i < PlayerInventoryStructs.Num(); ++i)
		{
			if (PlayerInventoryStructs[i].Name == ConsumableItemName)
			{
				const int32 ConsumableItemArrayIndex = i;
				if (PlayerInventoryStacks[ConsumableItemArrayIndex] > 1)
				{
					// 소모품의 수량이 1보다 많으면 하나 감소
					PlayerInventoryStacks[ConsumableItemArrayIndex]--;
					return;
				}
				// 소모품의 수량이 1개일 경우 기본 값으로 설정하고 수량 감소
				PlayerInventoryStructs[ConsumableItemArrayIndex] = InventoryStructDefault;
				PlayerInventoryStacks[ConsumableItemArrayIndex]--;
				return;
			}
		}
	}
}

void AEclipsePlayerState::DragFromDeadBody(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	// 사체에서 아이템을 드래그하는 함수 호출 - 서버 측 함수 호출
	DragFromDeadBodyServer(PlayerCharacterRef, DragArrayIndex, DropArrayIndex);
}

void AEclipsePlayerState::DragFromDeadBodyServer_Implementation(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	constexpr uint32 DeadBodySlotIndexNum_1 = 81; // 첫 번째 사체 인벤토리 슬롯 인덱스 시작 번호
	if (DropArrayIndex >= DeadBodySlotIndexNum_1) return; // 드롭 인덱스가 유효한 범위인지 확인
	if (DragArrayIndex >= 111)
	{
		// 사체 기어 슬롯에서 인벤토리로 아이템 이동
		if (constexpr uint32 DeadBodySlotIndexNum_2 = 111;
			PlayerInventoryStructs.IsValidIndex(DropArrayIndex) &&
			PlayerInventoryStacks.IsValidIndex(DropArrayIndex) &&
			DeadPlayerGearSlotStructs.IsValidIndex(DragArrayIndex - DeadBodySlotIndexNum_2))
		{
			// 인벤토리 슬롯에 아이템 복사 및 초기화
			PlayerInventoryStructs[DropArrayIndex] = DeadPlayerGearSlotStructs[DragArrayIndex - DeadBodySlotIndexNum_2];
			PlayerInventoryStacks[DropArrayIndex] = 1;
			DeadPlayerGearSlotStructs[DragArrayIndex - DeadBodySlotIndexNum_2] = InventoryStructDefault;
		}
	}
	else
	{
		// 사체 일반 인벤토리에서 인벤토리로 아이템 이동
		if (PlayerInventoryStructs.IsValidIndex(DropArrayIndex) &&
			PlayerInventoryStacks.IsValidIndex(DropArrayIndex) &&
			DeadPlayerInventoryStructs.IsValidIndex(DragArrayIndex - DeadBodySlotIndexNum_1))
		{
			// 인벤토리 슬롯에 아이템 복사 및 초기화
			PlayerInventoryStructs[DropArrayIndex] = DeadPlayerInventoryStructs[DragArrayIndex - DeadBodySlotIndexNum_1];
			PlayerInventoryStacks[DropArrayIndex] = DeadPlayerInventoryStacks[DragArrayIndex - DeadBodySlotIndexNum_1];
			DeadPlayerInventoryStructs[DragArrayIndex - DeadBodySlotIndexNum_1] = InventoryStructDefault;
			DeadPlayerInventoryStacks[DragArrayIndex - DeadBodySlotIndexNum_1] = 0;
		}
	}
	// 사체 컨테이너 참조가 유효한 경우 업데이트
	if (DeadPlayerContainerRef)
	{
		DeadPlayerContainerRef->DeadPlayerInventoryStructArray = DeadPlayerInventoryStructs;
		DeadPlayerContainerRef->DeadPlayerInventoryStackArray = DeadPlayerInventoryStacks;
		DeadPlayerContainerRef->DeadPlayerGearSlotArray = DeadPlayerGearSlotStructs;
	}
}

void AEclipsePlayerState::DragFromGearSlot(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	// 기어 슬롯에서 드래그하는 함수 호출 - 서버 측 함수 호출
	DragFromGearSlotServer(PlayerCharacterRef, DragArrayIndex, DropArrayIndex);
}

void AEclipsePlayerState::DragFromGearSlotServer_Implementation(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	// 플레이어 캐릭터가 유효한지 확인
	if (PlayerCharacterRef)
	{
		// 기어 슬롯 인덱스 범위 확인
		if (constexpr uint32 GearSlotIndexNum = 38;
			PlayerInventoryStructs.IsValidIndex(DropArrayIndex) &&
			PlayerInventoryStacks.IsValidIndex(DropArrayIndex) &&
			PlayerGearSlotStructs.IsValidIndex(DragArrayIndex - GearSlotIndexNum))
		{
			// 각 기어 슬롯의 아이템을 인벤토리로 이동
			if (DragArrayIndex == GearSlotIndexNum && PlayerInventoryStacks[DropArrayIndex] == 0)
			{
				PlayerCharacterRef->EquipHelmetInventorySlot(false, 1);
				PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - GearSlotIndexNum];
				PlayerInventoryStacks[DropArrayIndex]++;
				PlayerGearSlotStructs[DragArrayIndex - GearSlotIndexNum] = InventoryStructDefault;
			}
			else if (DragArrayIndex == 39 && PlayerInventoryStacks[DropArrayIndex] == 0)
			{
				PlayerCharacterRef->EquipGoggleInventorySlot(false, 1);
				PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - GearSlotIndexNum];
				PlayerInventoryStacks[DropArrayIndex]++;
				PlayerGearSlotStructs[DragArrayIndex - GearSlotIndexNum] = InventoryStructDefault;
			}
			else if (DragArrayIndex == 40 && PlayerInventoryStacks[DropArrayIndex] == 0)
			{
				PlayerCharacterRef->EquipArmorInventorySlot(false, PlayerGearSlotStructs[DragArrayIndex - GearSlotIndexNum].Stat);
				PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - GearSlotIndexNum];
				PlayerInventoryStacks[DropArrayIndex]++;
				PlayerGearSlotStructs[DragArrayIndex - GearSlotIndexNum] = InventoryStructDefault;
			}
			else if (DragArrayIndex == 41 && PlayerInventoryStacks[DropArrayIndex] == 0)
			{
				PlayerCharacterRef->EquipMaskInventorySlot(false, 1);
				PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - GearSlotIndexNum];
				PlayerInventoryStacks[DropArrayIndex]++;
				PlayerGearSlotStructs[DragArrayIndex - GearSlotIndexNum] = InventoryStructDefault;
			}
			else if (DragArrayIndex == 42 && PlayerInventoryStacks[DropArrayIndex] == 0)
			{
				PlayerCharacterRef->EquipHeadsetInventorySlot(false, 1);
				PlayerInventoryStructs[DropArrayIndex] = PlayerGearSlotStructs[DragArrayIndex - GearSlotIndexNum];
				PlayerInventoryStacks[DropArrayIndex]++;
				PlayerGearSlotStructs[DragArrayIndex - GearSlotIndexNum] = InventoryStructDefault;
			}
		}
	}
}

// 그라운드 아이템 리스트 드래그
void AEclipsePlayerState::DragFromGround(APlayerCharacter* PlayerCharacterRef, FPlayerInventoryStruct& PlayerInventoryStruct, const int32 DropArrayIndex, const bool IsAmmunition)
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
	// 인벤토리에서 드래그하는 함수 호출 - 서버 측 함수 호출
	DragFromInventoryServer(PlayerCharacterRef, DragArrayIndex, DropArrayIndex);
}

void AEclipsePlayerState::DragFromInventoryServer_Implementation(APlayerCharacter* PlayerCharacterRef, const int32 DragArrayIndex, const int32 DropArrayIndex)
{
	// 플레이어 캐릭터가 유효한지 확인
	if (PlayerCharacterRef)
	{
		// 드롭 인덱스가 헬멧 슬롯일 경우
		if (DropArrayIndex == 38)
		{
			// 아이템 타입이 헬멧인지 확인
			if (PlayerInventoryStructs[DragArrayIndex].ItemType == FString("Helmet"))
			{
				// 헬멧 장착
				PlayerCharacterRef->EquipHelmetInventorySlot(true, PlayerInventoryStructs[DragArrayIndex].Stat);
				if (PlayerInventoryStacks[DragArrayIndex] > 1)
				{
					// 헬멧 수량이 1보다 많으면 감소
					PlayerGearSlotStructs[0] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
				}
				else
				{
					// 헬멧 수량이 1개일 경우 슬롯 초기화
					PlayerGearSlotStructs[0] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
					PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
				}
			}
			return;
		}
		// 드롭 인덱스가 고글 슬롯일 경우
		if (DropArrayIndex == 39)
		{
			// 아이템 타입이 고글인지 확인
			if (PlayerInventoryStructs[DragArrayIndex].ItemType == FString("Goggle"))
			{
				// 고글 장착
				PlayerCharacterRef->EquipGoggleInventorySlot(true, PlayerInventoryStructs[DragArrayIndex].Stat);
				if (PlayerInventoryStacks[DragArrayIndex] > 1)
				{
					// 고글 수량이 1보다 많으면 감소
					PlayerGearSlotStructs[1] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
				}
				else
				{
					// 고글 수량이 1개일 경우 슬롯 초기화
					PlayerGearSlotStructs[1] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
					PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
				}
			}
			return;
		}
		// 드롭 인덱스가 갑옷 슬롯일 경우
		if (DropArrayIndex == 40)
		{
			// 아이템 타입이 갑옷인지 확인
			if (PlayerInventoryStructs[DragArrayIndex].ItemType == FString("Armor"))
			{
				// 갑옷 장착
				PlayerCharacterRef->EquipArmorInventorySlot(true, PlayerInventoryStructs[DragArrayIndex].Stat);
				if (PlayerInventoryStacks[DragArrayIndex] > 1)
				{
					// 갑옷 수량이 1보다 많으면 감소
					PlayerGearSlotStructs[2] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
				}
				else
				{
					// 갑옷 수량이 1개일 경우 슬롯 초기화
					PlayerGearSlotStructs[2] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
					PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
				}
			}
			return;
		}
		// 드롭 인덱스가 마스크 슬롯일 경우
		if (DropArrayIndex == 41)
		{
			// 아이템 타입이 마스크인지 확인
			if (PlayerInventoryStructs[DragArrayIndex].ItemType == FString("Mask"))
			{
				// 마스크 장착
				PlayerCharacterRef->EquipMaskInventorySlot(true, PlayerInventoryStructs[DragArrayIndex].Stat);
				if (PlayerInventoryStacks[DragArrayIndex] > 1)
				{
					// 마스크 수량이 1보다 많으면 감소
					PlayerGearSlotStructs[3] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
				}
				else
				{
					// 마스크 수량이 1개일 경우 슬롯 초기화
					PlayerGearSlotStructs[3] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
					PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
				}
			}
			return;
		}
		// 드롭 인덱스가 헤드셋 슬롯일 경우
		if (DropArrayIndex == 42)
		{
			// 아이템 타입이 헤드셋인지 확인
			if (PlayerInventoryStructs[DragArrayIndex].ItemType == FString("Headset"))
			{
				// 헤드셋 장착
				PlayerCharacterRef->EquipHeadsetInventorySlot(true, PlayerInventoryStructs[DragArrayIndex].Stat);
				if (PlayerInventoryStacks[DragArrayIndex] > 1)
				{
					// 헤드셋 수량이 1보다 많으면 감소
					PlayerGearSlotStructs[4] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
				}
				else
				{
					// 헤드셋 수량이 1개일 경우 슬롯 초기화
					PlayerGearSlotStructs[4] = PlayerInventoryStructs[DragArrayIndex];
					PlayerInventoryStacks[DragArrayIndex]--;
					PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
				}
			}
			return;
		}
		// 아이템을 드롭하는 경우
		if (DropArrayIndex == 45)
		{
			// 아이템 수량이 1보다 많으면 감소
			if (PlayerInventoryStacks[DragArrayIndex] > 1)
			{
				FActorSpawnParameters Param;
				Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				// 아이템 드롭 액터 생성
				GetWorld()->SpawnActor<AActor>(PlayerInventoryStructs[DragArrayIndex].ActorReference, PlayerCharacterRef->GetMesh()->GetSocketTransform(FName("ItemDrop")), Param);
				PlayerInventoryStacks[DragArrayIndex]--;
			}
			else
			{
				// 아이템 수량이 1개일 경우 슬롯 초기화
				FActorSpawnParameters Param;
				Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				// 아이템 드롭 액터 생성
				GetWorld()->SpawnActor<AActor>(PlayerInventoryStructs[DragArrayIndex].ActorReference, PlayerCharacterRef->GetMesh()->GetSocketTransform(FName("ItemDrop")), Param);
				PlayerInventoryStacks[DragArrayIndex]--;
				PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
			}
			return;
		}
		// 아이템을 이동하는 경우
		if (PlayerInventoryStacks[DropArrayIndex] == 0)
		{
			// 아이템 이동 및 슬롯 초기화
			PlayerInventoryStructs[DropArrayIndex] = PlayerInventoryStructs[DragArrayIndex];
			PlayerInventoryStacks[DropArrayIndex] = PlayerInventoryStacks[DragArrayIndex];
			PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
			PlayerInventoryStacks[DragArrayIndex] = 0;
			return;
		}
		// 아이템을 병합하는 경우
		if (PlayerInventoryStructs[DragArrayIndex].Name == PlayerInventoryStructs[DropArrayIndex].Name)
		{
			// 아이템 스택 증가 및 슬롯 초기화
			PlayerInventoryStacks[DropArrayIndex] += PlayerInventoryStacks[DragArrayIndex];
			PlayerInventoryStructs[DragArrayIndex] = InventoryStructDefault;
			return;
		}
		// 아이템을 교환하는 경우
		if (PlayerInventoryStructs[DragArrayIndex].Name != PlayerInventoryStructs[DropArrayIndex].Name)
		{
			// 아이템 교환
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
	// 서버에서 DeadBodyWidgetSettings 함수 호출
	DeadBodyWidgetSettingsServer(DeadPlayerContainer, InstigatorPlayerRef);
}

void AEclipsePlayerState::DeadBodyWidgetSettingsServer_Implementation(ADeadPlayerContainer* DeadPlayerContainer, APlayerCharacter* InstigatorPlayerRef)
{
	// DeadPlayerContainer가 유효한지 확인
	if (DeadPlayerContainer)
	{
		// DeadPlayerContainer 정보를 멤버 변수에 저장
		DeadPlayerContainerRef = DeadPlayerContainer;
		DeadPlayerInventoryStructs = DeadPlayerContainer->DeadPlayerInventoryStructArray;
		DeadPlayerInventoryStacks = DeadPlayerContainer->DeadPlayerInventoryStackArray;
		DeadPlayerGearSlotStructs = DeadPlayerContainer->DeadPlayerGearSlotArray;
	}

	// 멀티캐스트로 DeadBodyWidgetSettingsMulticast 함수 호출
	DeadBodyWidgetSettingsMulticast(DeadPlayerContainer, InstigatorPlayerRef);
}

void AEclipsePlayerState::DeadBodyWidgetSettingsMulticast_Implementation(ADeadPlayerContainer* DeadPlayerContainer, APlayerCharacter* InstigatorPlayerRef)
{
	// InstigatorPlayerRef가 유효하고 로컬 플레이어인지 확인
	if (InstigatorPlayerRef && InstigatorPlayerRef->IsLocallyControlled())
	{
		// 위젯 설정 함수 호출
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
	// 서버에서 ModifyRouble 함수 호출
	ModifyRoubleServer(PlayerCharacterRef, RoubleAmount);
}

void AEclipsePlayerState::ModifyRoubleServer_Implementation(APlayerCharacter* PlayerCharacterRef, const float RoubleAmount)
{
	// PlayerCharacterRef가 유효한지 확인
	if (PlayerCharacterRef)
	{
		// 플레이어의 루블 금액을 수정
		PlayerCharacterRef->Stat->AddRouble(RoubleAmount);
	}
}

void AEclipsePlayerState::ApplyGearInventoryEquipState(APlayerCharacter* PlayerCharacterRef)
{
	// PlayerGearSlotStructs 배열의 첫 번째 요소의 가격이 0보다 큰지 확인하고 헬멧 장착
	if (PlayerGearSlotStructs[0].Price > 0)
	{
		PlayerCharacterRef->EquipHelmetInventorySlot(false, 1);
	}

	// PlayerGearSlotStructs 배열의 두 번째 요소의 가격이 0보다 큰지 확인하고 고글 장착
	if (PlayerGearSlotStructs[1].Price > 0)
	{
		PlayerCharacterRef->EquipGoggleInventorySlot(false, 1);
	}

	// PlayerGearSlotStructs 배열의 세 번째 요소의 가격이 0보다 큰지 확인하고 갑옷 장착
	if (PlayerGearSlotStructs[2].Price > 0)
	{
		PlayerCharacterRef->EquipArmorInventorySlot(false, PlayerGearSlotStructs[2].Stat);
	}

	// PlayerGearSlotStructs 배열의 네 번째 요소의 가격이 0보다 큰지 확인하고 마스크 장착
	if (PlayerGearSlotStructs[3].Price > 0)
	{
		PlayerCharacterRef->EquipMaskInventorySlot(false, 1);
	}

	// PlayerGearSlotStructs 배열의 다섯 번째 요소의 가격이 0보다 큰지 확인하고 헤드셋 장착
	if (PlayerGearSlotStructs[4].Price > 0)
	{
		PlayerCharacterRef->EquipHeadsetInventorySlot(false, 1);
	}
}

void AEclipsePlayerState::ResetPlayerInventoryData()
{
	// 플레이어 인벤토리 데이터 초기화
	PlayerInventoryStructs.Init(InventoryStructDefault, 30); // 인벤토리 구조체 배열 초기화
	PlayerInventoryStacks.Init(0, 30); // 인벤토리 스택 배열 초기화
	PlayerGearSlotStructs.Init(InventoryStructDefault, 5); // 기어 슬롯 배열 초기화
}
