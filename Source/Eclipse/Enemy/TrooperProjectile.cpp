// Fill out your copyright notice in the Description page of Project Settings.


#include "TrooperProjectile.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATrooperProjectile::ATrooperProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	SetRootComponent(BulletMesh);
	BulletMesh->SetGenerateOverlapEvents(true);

	bReplicates = true;
	SetReplicatingMovement(true);

}


// Called when the game starts or when spawned
void ATrooperProjectile::BeginPlay()
{
	Super::BeginPlay();

	BulletMesh->OnComponentBeginOverlap.AddDynamic(this, &ATrooperProjectile::OnOverlap);
	this->SetLifeSpan(5.0f);
	
}

void ATrooperProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (const auto Player = Cast<APlayerCharacter>(OtherActor); Player&&Player->IsPlayerDead==false)
		{
			if(Player->HasAuthority()&&Player->IsPlayerDeadImmediately==false)
			{
				Player->Damaged(TrooperDamageValue, this);
				Destroy();
			}
		}
	}
	if (OtherComp)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), TrooperGroundHitSound, this->GetActorLocation());
		Destroy();
	}
}

