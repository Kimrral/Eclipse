// Fill out your copyright notice in the Description page of Project Settings.


#include "PistolAmmoActor.h"

#include "InformationWidget.h"
#include "PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

void APistolAmmoActor::BeginPlay()
{
	Super::BeginPlay();

	ammoMesh->OnComponentBeginOverlap.AddDynamic(this, &APistolAmmoActor::OnOverlap);
	playerTraceSphereCollision->OnComponentBeginOverlap.AddDynamic(this, &APistolAmmoActor::SphereOnOverlap);

}

void APistolAmmoActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bTracePlayer)
	{
		auto target = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());
		if (target != nullptr)
		{
			// 플레이어의 위치 - 나의 위치 = 갈 방향을 설정한다.
			FVector targetDir = target->GetActorLocation() - GetActorLocation();
			targetDir.Normalize();
			direction = targetDir;
		}
		SetActorLocation(GetActorLocation() + direction * moveSpeed * DeltaSeconds);
	}
}

void APistolAmmoActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		auto player=Cast<APlayerCharacter>(OtherActor);
		if(player)
		{
			player->maxPistolAmmo+=8;
			player->informationUI->UpdateAmmo_Secondary();

			this->Destroy();
		}
	}
}

void APistolAmmoActor::SphereOnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		auto player=Cast<APlayerCharacter>(OtherActor);
		if(player)
		{
			bTracePlayer=true;
			ammoMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
		}
	}
}

