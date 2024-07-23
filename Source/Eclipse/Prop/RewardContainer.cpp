// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardContainer.h"


#include "Eclipse/Item/RewardManagerComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// 기본 값을 설정합니다
ARewardContainer::ARewardContainer()
{
	// 이 액터가 매 프레임마다 Tick()을 호출하도록 설정합니다. 필요하지 않은 경우 성능 향상을 위해 끌 수 있습니다.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// 루트 메쉬 컴포넌트를 생성하고 루트 컴포넌트로 설정합니다.
	ContainerMesh = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("containerMesh"));
	SetRootComponent(ContainerMesh);
	
	// 보상 매니저 컴포넌트를 생성합니다.
	RewardManager = CreateDefaultSubobject<URewardManagerComponent>(TEXT("RewardManagerComponent"));

	// 현재 박스 HP를 최대 HP로 설정합니다.
	CurBoxHP = MaxBoxHP;
	
	// 이 액터가 네트워크 상에서 복제되도록 설정합니다.
	bReplicates = true;
}

// 이 액터의 복제된 속성을 설정합니다.
void ARewardContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// IsBoxDestroyed 변수를 네트워크 상에서 복제되도록 설정합니다.
	DOREPLIFETIME(ARewardContainer, IsBoxDestroyed);
}

// 게임이 시작되거나 액터가 스폰될 때 호출됩니다
void ARewardContainer::BeginPlay()
{
	Super::BeginPlay();

	// 컨테이너 메쉬에 커스텀 뎁스 렌더링을 설정합니다.
	ContainerMesh->SetRenderCustomDepth(true);
	// 컨테이너 메쉬가 물리를 시뮬레이션하도록 설정합니다.
	ContainerMesh->SetSimulatePhysics(true);
}

// 박스가 파괴되었을 때 호출됩니다
void ARewardContainer::BoxDestroyed()
{
	// 박스가 파괴되었음을 설정하고 이를 복제합니다.
	IsBoxDestroyed = true;
	OnRep_IsBoxDestroyed();
	// 보상을 드랍합니다.
	RewardManager->DropRewardServer(GetActorTransform());	
}

// IsBoxDestroyed 변수가 복제되었을 때 호출됩니다
void ARewardContainer::OnRep_IsBoxDestroyed()
{
	// 이 액터의 수명을 5초로 설정합니다.
	SetLifeSpan(5.f);
	// 박스가 파괴된 위치에서 이벤트를 실행합니다.
	containerDele.ExecuteIfBound(GetActorLocation());
	// 박스 파괴 사운드를 재생합니다.
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ContainerBreakSound, GetActorLocation());
}
