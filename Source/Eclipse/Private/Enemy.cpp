// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "EclipsePlayerController.h"
#include "EnemyAnim.h"
#include "EnemyFSM.h"
#include "EnemyHPWidget.h"
#include "M249AmmoActor.h"
#include "PistolAmmoActor.h"
#include "PlayerCharacter.h"
#include "RifleAmmoActor.h"
#include "SniperAmmoActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Eclipse/EclipseGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Enemy FSM
	enemyFSM=CreateDefaultSubobject<UEnemyFSM>(TEXT("enemyFSM"));
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Set HP
	curHP=maxHP;

	enemyAnim = Cast<UEnemyAnim>(GetMesh()->GetAnimInstance());
	gameMode = Cast<AEclipseGameMode>(GetWorld()->GetAuthGameMode());
	PC = Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//DetectPlayerLineTrace();

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::Move()
{
}


void AEnemy::OnDie()
{
	FTimerHandle destroyHandle;
	GetCharacterMovement()->Deactivate();
	StopAnimMontage();
	auto capsule = GetCapsuleComponent();
	capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().SetTimer(destroyHandle, this, &AEnemy::OnDestroy, 10.0f, false);
}

void AEnemy::OnDamaged()
{
	FTimerHandle overlayMatHandle;
	GetMesh()->SetOverlayMaterial(overlayMatRed);
	GetWorldTimerManager().ClearTimer(overlayMatHandle);
	GetWorldTimerManager().SetTimer(overlayMatHandle, FTimerDelegate::CreateLambda([this]()->void
	{
		GetMesh()->SetOverlayMaterial(nullptr);
	}), 0.3f, false);
}

void AEnemy::OnHeadDamaged()
{
	FTimerHandle overlayMatHandle;
	//GetController()->StopMovement();
	//GetCharacterMovement()->Deactivate();
	GetMesh()->SetOverlayMaterial(overlayMatRed);
	//PlayAnimMontage(damageMontage, 1);
	GetWorldTimerManager().ClearTimer(overlayMatHandle);
	GetWorldTimerManager().SetTimer(overlayMatHandle, FTimerDelegate::CreateLambda([this]()->void
	{
		GetMesh()->SetOverlayMaterial(nullptr);
	}), 0.3f, false);
}

void AEnemy::OnDestroy()
{
	this->Destroy();
}

void AEnemy::SetHPWidgetInvisible()
{	
	GetWorldTimerManager().SetTimer(HPWidgetInvisibleHandle, FTimerDelegate::CreateLambda([this]()->void
	{
	}), 3.0f, false);
}

void AEnemy::DropReward()
{
	auto randIndex = FMath::RandRange(0, 3);
	if(randIndex==0)
	{
		auto rifleAmmo = GetWorld()->SpawnActor<ARifleAmmoActor>(rifleAmmoFactory, GetActorLocation(), GetActorRotation());
		if(rifleAmmo)
		{
			FVector force = FVector(0, 0, 5000);
			FVector loc = rifleAmmo->GetActorUpVector();
			rifleAmmo->ammoMesh->AddImpulseAtLocation(force, loc);
		}
	}
	else if(randIndex==1)
	{
		auto sniperAmmo = GetWorld()->SpawnActor<ASniperAmmoActor>(sniperAmmoFactory, GetActorLocation(), GetActorRotation());
		if(sniperAmmo)
		{
			FVector force = FVector(0, 0, 5000);
			FVector loc = sniperAmmo->GetActorUpVector();
			sniperAmmo->ammoMesh->AddImpulseAtLocation(force, loc);
		}
	}
	else if(randIndex==2)
	{
		auto pistolAmmo = GetWorld()->SpawnActor<APistolAmmoActor>(pistolAmmoFactory, GetActorLocation(), GetActorRotation());
		if(pistolAmmo)
		{
			FVector force = FVector(0, 0, 5000);
			FVector loc = pistolAmmo->GetActorUpVector();
			pistolAmmo->ammoMesh->AddImpulseAtLocation(force, loc);
		}
	}
	else if(randIndex==3)
	{
		auto m249Ammo = GetWorld()->SpawnActor<AM249AmmoActor>(M249AmmoFactory, GetActorLocation(), GetActorRotation());
		if(m249Ammo)
		{
			FVector force = FVector(0, 0, 5000);
			FVector loc = m249Ammo->GetActorUpVector();
			m249Ammo->ammoMesh->AddImpulseAtLocation(force, loc);
		}
	}
}

void AEnemy::DetectPlayerLineTrace()
{
	if(enemyFSM->state==EEnemyState::MOVE||enemyFSM->state==EEnemyState::ATTACK)
	{
		FHitResult enemySightResult;
		FVector startPos = GetActorLocation();
		FVector endPos = enemyFSM->player->GetActorLocation();
		//DrawDebugLine(GetWorld(), startPos, endPos, FColor::Red);
		bool bHit = GetWorld()->LineTraceSingleByChannel(enemySightResult, startPos, endPos, ECollisionChannel::ECC_Visibility);
		if(bHit)
		{
			auto player = Cast<APlayerCharacter>(enemySightResult.GetActor());
			if(player)
			{
				bPlayerInSight=true;
				UE_LOG(LogTemp, Warning, TEXT("Player in Sight"))
			}
			else
			{
				bPlayerInSight=false;
				UE_LOG(LogTemp, Warning, TEXT("Player out Sight"))
			}
		}
	}
}

void AEnemy::EnemyAttackProcess()
{
	auto particleTrans=GetMesh()->GetSocketTransform(FName("Muzzle"));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireParticle, particleTrans);

}

