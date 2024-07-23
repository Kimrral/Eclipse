// Fill out your copyright notice in the Description page of Project Settings.


#include "TrooperProjectile.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// 기본 값을 설정합니다
ATrooperProjectile::ATrooperProjectile()
{
	// 이 액터가 매 프레임마다 Tick()을 호출하도록 설정합니다. 필요하지 않은 경우 성능 향상을 위해 끌 수 있습니다.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// BulletMesh 컴포넌트를 생성하고 루트 컴포넌트로 설정합니다.
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	SetRootComponent(BulletMesh);
	
	// 충돌 이벤트를 생성하도록 설정합니다.
	BulletMesh->SetGenerateOverlapEvents(true);

	// 이 액터가 네트워크 상에서 복제되도록 설정합니다.
	bReplicates = true;
}

// 게임이 시작되거나 액터가 스폰될 때 호출됩니다
void ATrooperProjectile::BeginPlay()
{
	Super::BeginPlay();

	// BulletMesh에 겹침 이벤트가 발생했을 때 OnOverlap 함수를 호출하도록 설정합니다.
	BulletMesh->OnComponentBeginOverlap.AddDynamic(this, &ATrooperProjectile::OnOverlap);
	
	// 이 액터의 수명을 5초로 설정합니다.
	this->SetLifeSpan(5.0f);
}

// 겹침 이벤트가 발생했을 때 호출됩니다
void ATrooperProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// 겹친 액터가 플레이어 캐릭터인지 확인하고, 플레이어가 죽지 않은 상태인지 확인합니다.
		if (const auto Player = Cast<APlayerCharacter>(OtherActor); Player && Player->IsPlayerDead == false)
		{
			// 플레이어가 서버 권한을 가지고 있으며 즉시 죽지 않은 상태인지 확인합니다.
			if (Player->HasAuthority() && Player->IsPlayerDeadImmediately == false)
			{
				// 플레이어에게 피해를 주고 이 액터를 파괴합니다.
				Player->Damaged(TrooperDamageValue, this);
				Destroy();
			}
		}
	}

	if (OtherComp)
	{
		// 겹친 컴포넌트가 있을 경우, 충돌 사운드를 재생하고 이 액터를 파괴합니다.
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), TrooperGroundHitSound, GetActorLocation());
		Destroy();
	}
}
