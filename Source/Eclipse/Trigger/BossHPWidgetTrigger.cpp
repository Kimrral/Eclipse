// Fill out your copyright notice in the Description page of Project Settings.


#include "BossHPWidgetTrigger.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Eclipse/Manager/EnemyHpWidgetControllerComponent.h"
#include "Eclipse/Manager/InventoryControllerComponent.h"
#include "Eclipse/Player/EclipsePlayerController.h"

// 기본값을 설정합니다
ABossHPWidgetTrigger::ABossHPWidgetTrigger()
{
	// 이 액터가 매 프레임마다 Tick()을 호출하도록 설정합니다. 성능을 향상시키기 위해 필요하지 않다면 이를 비활성화할 수 있습니다.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// BoxCollision 컴포넌트를 생성하고 루트 컴포넌트로 설정합니다.
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("boxCollision"));
	SetRootComponent(BoxCollision);
	// BoxCollision이 겹침 이벤트를 생성하도록 설정합니다.
	BoxCollision->SetGenerateOverlapEvents(true);
}

// 게임이 시작되거나 스폰되었을 때 호출됩니다
void ABossHPWidgetTrigger::BeginPlay()
{
	Super::BeginPlay();

	// BoxCollision의 겹침 시작 이벤트와 끝 이벤트에 함수를 바인딩합니다.
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossHPWidgetTrigger::OnOverlap);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ABossHPWidgetTrigger::EndOverlap);
}

// 겹침 시작 시 호출되는 함수입니다
void ABossHPWidgetTrigger::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// 겹친 액터가 플레이어 캐릭터인지 확인합니다.
		if (const auto Player = Cast<APlayerCharacter>(OtherActor))
		{
			// 플레이어가 유효하고 로컬 컨트롤된 플레이어인지 확인합니다.
			if (::IsValid(Player) && Player->IsLocallyControlled())
			{				
				// 플레이어의 PC(Player Controller)를 통해 보스 HP 위젯을 화면에 추가합니다.
				Player->PC->EnemyHpWidgetController->AddBossHpWidgetToViewport();				
			}
		}
	}
}

// 겹침 끝날 시 호출되는 함수입니다
void ABossHPWidgetTrigger::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		// 겹침이 끝난 액터가 플레이어 캐릭터인지 확인합니다.
		if (const auto Player = Cast<APlayerCharacter>(OtherActor))
		{
			// 플레이어가 유효하고 로컬 컨트롤된 플레이어인지 확인합니다.
			if (::IsValid(Player) && Player->IsLocallyControlled())
			{				
				// 플레이어의 PC(Player Controller)를 통해 보스 HP 위젯을 화면에서 제거합니다.
				Player->PC->EnemyHpWidgetController->RemoveBossHpWidgetFromViewport();
			}
		}
	}
}


