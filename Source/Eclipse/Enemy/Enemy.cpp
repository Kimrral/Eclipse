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
#include "Components/ProgressBar.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
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
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Enemy FSM
	EnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("enemyFSM"));
	// Pawn Sensor
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	// Stat Component 
	EnemyStat = CreateDefaultSubobject<UEnemyCharacterStatComponent>(TEXT("Stat"));

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicateMovement(true);

	EnemyStat->OnShieldZero.AddUObject(this, &AEnemy::OnShieldDestroy);

	EnemyAnim = Cast<UEnemyAnim>(GetMesh()->GetAnimInstance());
	gameMode = Cast<AEclipseGameMode>(GetWorld()->GetAuthGameMode());
	PC = Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());
}

void AEnemy::OnDie()
{
	FTimerHandle destroyHandle;
	EnemyFSM->Timeline.Stop();
	EnemyStat->IsStunned = false;
	StopAnimMontage();
	GetWorld()->GetTimerManager().ClearTimer(StunHandle);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->Deactivate();
	StopAnimMontage();
	UCapsuleComponent* const capsule = GetCapsuleComponent();
	capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().SetTimer(destroyHandle, this, &AEnemy::OnDestroy, 10.0f, false);
}

void AEnemy::Damaged(int damage, AActor* DamageCauser)
{
	DamagedRPCServer(damage, DamageCauser);
	EnemyStat->ApplyDamage(damage, DamageCauser);
}

void AEnemy::DamagedRPCServer_Implementation(int damage, AActor* DamageCauser)
{
	DamagedRPCMulticast(damage, DamageCauser);
}

bool AEnemy::DamagedRPCServer_Validate(int damage, AActor* DamageCauser)
{
	return true;
}

void AEnemy::DamagedRPCMulticast_Implementation(int damage, AActor* DamageCauser)
{
	FTimerHandle overlayMatHandle;
	GetMesh()->SetOverlayMaterial(overlayMatRed);
	GetWorldTimerManager().ClearTimer(overlayMatHandle);
	GetWorldTimerManager().SetTimer(overlayMatHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		GetMesh()->SetOverlayMaterial(nullptr);
	}), 0.3f, false);
}

void AEnemy::OnShieldDestroy()
{
	EnemyStat->IsShieldBroken = true;
	EnemyStat->IsStunned = true;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), EnemyFSM->ShieldBreakSound, GetActorLocation(), FRotator::ZeroRotator);
	auto EmitterTrans = GetMesh()->GetSocketTransform(FName("ShieldSocket"));
	EmitterTrans.SetScale3D(FVector(4));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyFSM->ShieldBreakEmitter, EmitterTrans);
	// 움직임 즉시 중단
	GetCharacterMovement()->StopMovementImmediately();
	// Movement Mode = None [움직임 차단]
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	StopAnimMontage();
	PlayAnimMontage(stunMontage, 1, FName("StunStart"));
	GetWorld()->GetTimerManager().SetTimer(StunHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		EnemyStat->IsStunned = false;
		StopAnimMontage();
		// Movement Mode = Walking [움직임 재개]
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		// Shield 회복
		EnemyStat->SetShield(EnemyStat->GetMaxShield());
		EnemyStat->IsShieldBroken = false;
		EnemyFSM->player->bossHPUI->shieldProgressBar->SetPercent(1);
		EnemyFSM->SetState(EEnemyState::MOVE);
	}), 7.0f, false);
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
	if (randIndex == 0)
	{
		GetWorld()->SpawnActor<ARifleAmmoActor>(rifleAmmoFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (randIndex == 1)
	{
		GetWorld()->SpawnActor<ASniperAmmoActor>(sniperAmmoFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (randIndex == 2)
	{
		GetWorld()->SpawnActor<APistolAmmoActor>(pistolAmmoFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (randIndex == 3)
	{
		GetWorld()->SpawnActor<AM249AmmoActor>(M249AmmoFactory, GetActorLocation(), GetActorRotation(), param);
	}
}

void AEnemy::DropMagazine()
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto randIndex = FMath::RandRange(0, 3);
	if (randIndex == 0)
	{
		GetWorld()->SpawnActor<ARifleMagActor>(RifleMagActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (randIndex == 1)
	{
		GetWorld()->SpawnActor<ASniperMagActor>(SniperMagActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (randIndex == 2)
	{
		GetWorld()->SpawnActor<APistolMagActor>(PistolMagActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (randIndex == 3)
	{
		GetWorld()->SpawnActor<AM249MagActor>(M249MagActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
}

void AEnemy::DropGear()
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto randIndex = FMath::RandRange(0, 4);
	if (randIndex == 0)
	{
		GetWorld()->SpawnActor<AHelmetActor>(HelmetActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (randIndex == 1)
	{
		GetWorld()->SpawnActor<AHeadsetActor>(HeadsetActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (randIndex == 2)
	{
		GetWorld()->SpawnActor<AMaskActor>(MaskActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (randIndex == 3)
	{
		GetWorld()->SpawnActor<AGoggleActor>(GoggleActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (randIndex == 4)
	{
		GetWorld()->SpawnActor<AArmorActor>(ArmorActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
}

void AEnemy::GuardianFireProcess()
{
	if (EnemyFSM->player)
	{
		const FTransform muzzleTrans = GetMesh()->GetSocketTransform(FName("Muzzle"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireParticle, muzzleTrans);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GuardianFireSound, this->GetActorLocation());
		const FVector playerLoc = (EnemyFSM->player->GetActorLocation() - muzzleTrans.GetLocation());
		const FRotator projectileRot = UKismetMathLibrary::MakeRotFromXZ(playerLoc, this->GetActorUpVector());
		GetWorld()->SpawnActor<AGuardianProjectile>(GuardianProjectileFactory, muzzleTrans.GetLocation(), projectileRot);
	}
}
