// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardianProjectile.h"

#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGuardianProjectile::AGuardianProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rocketMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("rocketMesh"));
	SetRootComponent(rocketMesh);
	rocketMesh->SetGenerateOverlapEvents(true);
}


// Called when the game starts or when spawned
void AGuardianProjectile::BeginPlay()
{
	Super::BeginPlay();

	rocketMesh->OnComponentBeginOverlap.AddDynamic(this, &AGuardianProjectile::OnOverlap);
	this->SetLifeSpan(5.0f);
	
}

void AGuardianProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), explosionSound, GetActorLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionParticle, GetActorLocation());
}

// Called every frame
void AGuardianProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGuardianProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		auto player=Cast<APlayerCharacter>(OtherActor);
		if(player)
		{			
			player->Damaged(guardianDamageValue);
			Explosion();
		}
	}
	if(OtherComp)
	{
		Explosion();
		// 오버랩 멀티 중심점
		FVector Center = GetActorLocation();
		// 충돌체크(구충돌)
		// 충돌한 물체를 기억할 배열
		TArray<FOverlapResult> HitObj;
		FCollisionQueryParams params;
		// 충돌 무시 액터 리스트 추가
		params.AddIgnoredActor(this);
		// 집게를 든채로 컨테이너에서 Trigger 되었는지 판단하는 OverlapMulti
		DrawDebugSphere(GetWorld(), Center, 100, 30, FColor::Red, false, 2);
		bool bHitVat = GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(100), params);
		// OverlapMulti가 적중하지 않았다면
		if (bHitVat == false)
		{
			// 리턴시킨다.
			return;
		}
		for (int i = 0; i < HitObj.Num(); ++i)
		{
			auto player=Cast<APlayerCharacter>(OtherActor);
			if(player)
			{
				auto dist = GetDistanceTo(player);
				// 폭발 중심점부터의 거리에 따른 데미지 프로세스
				player->Damaged(FMath::Clamp(guardianDamageValue-(dist*2), 0, 30));
			}
		}		
	}
}

void AGuardianProjectile::Explosion()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), explosionSound, GetActorLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionParticle, GetActorLocation(), FRotator::ZeroRotator, FVector(0.2));
	this->Destroy();
}
