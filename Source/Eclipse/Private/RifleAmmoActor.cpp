// Fill out your copyright notice in the Description page of Project Settings.


#include "RifleAmmoActor.h"

#include "InformationWidget.h"
#include "PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

void ARifleAmmoActor::BeginPlay()
{
	Super::BeginPlay();

	ammoMesh->OnComponentBeginOverlap.AddDynamic(this, &ARifleAmmoActor::OnOverlap);
	playerTraceSphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ARifleAmmoActor::SphereOnOverlap);
}

void ARifleAmmoActor::Tick(float DeltaSeconds)
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

void ARifleAmmoActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		auto player=Cast<APlayerCharacter>(OtherActor);
		if(player)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ammoPickupSound);
			player->maxRifleAmmo+=40;
			player->informationUI->UpdateAmmo_Secondary();

			this->Destroy();
		}
	}
}

void ARifleAmmoActor::SphereOnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		auto player=Cast<APlayerCharacter>(OtherActor);
		if(player)
		{
			SetActorTickEnabled(true);
			bTracePlayer=true;
			ammoMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
			ammoMesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
		}
	}
}
