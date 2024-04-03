// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardianProjectile.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGuardianProjectile::AGuardianProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	rocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("rocketMesh"));
	SetRootComponent(rocketMesh);
	rocketMesh->SetGenerateOverlapEvents(true);

	bReplicates = true;
	SetReplicatingMovement(true);
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
	if (OtherActor)
	{
		auto player = Cast<APlayerCharacter>(OtherActor);
		if (player&&player->IsPlayerDead==false)
		{
			if(player->HasAuthority()&&player->IsPlayerDeadImmediately==false)
			{
				player->Damaged(guardianDamageValue, this);
				Explosion();
			}
		}
	}
	if (OtherComp)
	{
		Explosion();
	}
}

void AGuardianProjectile::Explosion()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), explosionSound, GetActorLocation(), 0.5f);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionParticle, GetActorLocation(), FRotator::ZeroRotator, FVector(0.05f));
	this->Destroy();
}
