// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Eclipse/Item/ArmorActor.h"
#include "Eclipse/Player/EclipsePlayerController.h"
#include "Eclipse/Animation/EnemyAnim.h"
#include "Eclipse/AI/EnemyFSM.h"
#include "Eclipse/Item/GoggleActor.h"
#include "GuardianProjectile.h"
#include "Eclipse/Item/HeadsetActor.h"
#include "Eclipse/Item/HelmetActor.h"
#include "Eclipse/Item/M249AmmoActor.h"
#include "Eclipse/Item/M249MagActor.h"
#include "Eclipse/Item/MaskActor.h"
#include "Eclipse/Item/PistolAmmoActor.h"
#include "Eclipse/Item/PistolMagActor.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/Item/RifleAmmoActor.h"
#include "Eclipse/Item/RifleMagActor.h"
#include "Eclipse/Item/SniperAmmoActor.h"
#include "Eclipse/Item/SniperMagActor.h"
#include "Components/CapsuleComponent.h"
#include "Eclipse/Game/EclipseGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled=false;

	// Enemy FSM
	enemyFSM=CreateDefaultSubobject<UEnemyFSM>(TEXT("enemyFSM"));

	PawnSensingComponent=CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	bReplicates=true;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Set HP
	curHP=maxHP;
	// Set Shield
	curShield=maxShield;

	enemyAnim = Cast<UEnemyAnim>(GetMesh()->GetAnimInstance());
	gameMode = Cast<AEclipseGameMode>(GetWorld()->GetAuthGameMode());
	PC = Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	enemyFSM->Timeline.Stop();
	isStunned=false;
	StopAnimMontage();
	GetWorld()->GetTimerManager().ClearTimer(enemyFSM->stunHandle);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
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
	GetMesh()->SetOverlayMaterial(overlayMatRed);
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

void AEnemy::DropReward()
{

}

void AEnemy::DropAmmo()
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto randIndex = FMath::RandRange(0, 3);
	if(randIndex==0)
	{
		GetWorld()->SpawnActor<ARifleAmmoActor>(rifleAmmoFactory, GetActorLocation(), GetActorRotation(), param);		
	}
	else if(randIndex==1)
	{
		GetWorld()->SpawnActor<ASniperAmmoActor>(sniperAmmoFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if(randIndex==2)
	{
		GetWorld()->SpawnActor<APistolAmmoActor>(pistolAmmoFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if(randIndex==3)
	{
		GetWorld()->SpawnActor<AM249AmmoActor>(M249AmmoFactory, GetActorLocation(), GetActorRotation(), param);
	}
}

void AEnemy::DropMagazine()
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto randIndex = FMath::RandRange(0, 3);
	if(randIndex==0)
	{
		GetWorld()->SpawnActor<ARifleMagActor>(RifleMagActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if(randIndex==1)
	{
		GetWorld()->SpawnActor<ASniperMagActor>(SniperMagActorFactory, GetActorLocation(), GetActorRotation(), param);	
	}
	else if(randIndex==2)
	{
		GetWorld()->SpawnActor<APistolMagActor>(PistolMagActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if(randIndex==3)
	{
		GetWorld()->SpawnActor<AM249MagActor>(M249MagActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
}

void AEnemy::DropGear()
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto randIndex = FMath::RandRange(0, 4);
	if(randIndex==0)
	{
		GetWorld()->SpawnActor<AHelmetActor>(HelmetActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if(randIndex==1)
	{
		GetWorld()->SpawnActor<AHeadsetActor>(HeadsetActorFactory, GetActorLocation(), GetActorRotation(), param);	
	}
	else if(randIndex==2)
	{
		GetWorld()->SpawnActor<AMaskActor>(MaskActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if(randIndex==3)
	{
		GetWorld()->SpawnActor<AGoggleActor>(GoggleActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if(randIndex==4)
	{
		GetWorld()->SpawnActor<AArmorActor>(ArmorActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
}

void AEnemy::GuardianFireProcess()
{	
	if(enemyFSM->player)
	{
		auto muzzleTrans=GetMesh()->GetSocketTransform(FName("Muzzle"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireParticle, muzzleTrans);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GuardianFireSound, this->GetActorLocation());
		FVector playerLoc = (enemyFSM->player->GetActorLocation() - muzzleTrans.GetLocation());
		auto projectileRot = UKismetMathLibrary::MakeRotFromXZ(playerLoc, this->GetActorUpVector());
		GetWorld()->SpawnActor<AGuardianProjectile>(GuardianProjectileFactory, muzzleTrans.GetLocation(), projectileRot);
	}
}

