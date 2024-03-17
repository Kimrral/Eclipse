// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "ArmorActor.h"
#include "EclipsePlayerController.h"
#include "EnemyAnim.h"
#include "EnemyFSM.h"
#include "GoggleActor.h"
#include "GuardianProjectile.h"
#include "HeadsetActor.h"
#include "HelmetActor.h"
#include "M249AmmoActor.h"
#include "M249MagActor.h"
#include "MaskActor.h"
#include "PistolAmmoActor.h"
#include "PistolMagActor.h"
#include "PlayerCharacter.h"
#include "RifleAmmoActor.h"
#include "RifleMagActor.h"
#include "SniperAmmoActor.h"
#include "SniperMagActor.h"
#include "Components/CapsuleComponent.h"
#include "Eclipse/EclipseGameMode.h"
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
		auto rifleAmmo = GetWorld()->SpawnActor<ARifleAmmoActor>(rifleAmmoFactory, GetActorLocation(), GetActorRotation(), param);
		if(rifleAmmo)
		{
			//rifleAmmo->ammoMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
	}
	else if(randIndex==1)
	{
		auto sniperAmmo = GetWorld()->SpawnActor<ASniperAmmoActor>(sniperAmmoFactory, GetActorLocation(), GetActorRotation(), param);
		if(sniperAmmo)
		{
			//sniperAmmo->ammoMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
	}
	else if(randIndex==2)
	{
		auto pistolAmmo = GetWorld()->SpawnActor<APistolAmmoActor>(pistolAmmoFactory, GetActorLocation(), GetActorRotation(), param);
		if(pistolAmmo)
		{
			//pistolAmmo->ammoMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
	}
	else if(randIndex==3)
	{
		auto m249Ammo = GetWorld()->SpawnActor<AM249AmmoActor>(M249AmmoFactory, GetActorLocation(), GetActorRotation(), param);
		if(m249Ammo)
		{
			//m249Ammo->ammoMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
	}
}

void AEnemy::DropMagazine()
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto randIndex = FMath::RandRange(0, 3);
	if(randIndex==0)
	{
		auto RifleMagActor = GetWorld()->SpawnActor<ARifleMagActor>(RifleMagActorFactory, GetActorLocation(), GetActorRotation(), param);
		if(RifleMagActor)
		{
			//RifleMagActor->magMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
	}
	else if(randIndex==1)
	{
		auto SniperMagActor = GetWorld()->SpawnActor<ASniperMagActor>(SniperMagActorFactory, GetActorLocation(), GetActorRotation(), param);
		if(SniperMagActor)
		{
			//SniperMagActor->magMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
	}
	else if(randIndex==2)
	{
		auto PistolMagActor = GetWorld()->SpawnActor<APistolMagActor>(PistolMagActorFactory, GetActorLocation(), GetActorRotation(), param);
		if(PistolMagActor)
		{
			//PistolMagActor->magMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
	}
	else if(randIndex==3)
	{
		auto M249MagActor = GetWorld()->SpawnActor<AM249MagActor>(M249MagActorFactory, GetActorLocation(), GetActorRotation(), param);
		if(M249MagActor)
		{
			//M249MagActor->magMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
	}
}

void AEnemy::DropGear()
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto randIndex = FMath::RandRange(0, 4);
	if(randIndex==0)
	{
		auto HelmetActor = GetWorld()->SpawnActor<AHelmetActor>(HelmetActorFactory, GetActorLocation(), GetActorRotation(), param);
		if(HelmetActor)
		{
			//HelmetActor->gearMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
	}
	else if(randIndex==1)
	{
		auto HeadsetActor = GetWorld()->SpawnActor<AHeadsetActor>(HeadsetActorFactory, GetActorLocation(), GetActorRotation(), param);
		if(HeadsetActor)
		{
			//HeadsetActor->gearMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
	}
	else if(randIndex==2)
	{
		auto MaskActor = GetWorld()->SpawnActor<AMaskActor>(MaskActorFactory, GetActorLocation(), GetActorRotation(), param);
		if(MaskActor)
		{
			//MaskActor->gearMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
	}
	else if(randIndex==3)
	{
		auto GoggleActor = GetWorld()->SpawnActor<AGoggleActor>(GoggleActorFactory, GetActorLocation(), GetActorRotation(), param);
		if(GoggleActor)
		{
			//GoggleActor->gearMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
	}
	else if(randIndex==4)
	{
		auto ArmorActor = GetWorld()->SpawnActor<AArmorActor>(ArmorActorFactory, GetActorLocation(), GetActorRotation(), param);
		if(ArmorActor)
		{
			//ArmorActor->gearMesh->AddImpulseAtLocation(DropForce, DropLoc);
		}
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

