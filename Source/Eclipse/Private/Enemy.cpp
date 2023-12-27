// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "ArmorActor.h"
#include "EclipsePlayerController.h"
#include "EnemyAnim.h"
#include "EnemyFSM.h"
#include "GoggleActor.h"
#include "HeadsetActor.h"
#include "HelmetActor.h"
#include "M249AmmoActor.h"
#include "M249MagActor.h"
#include "MaskActor.h"
#include "NavigationInvokerComponent.h"
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
#include "Perception/PawnSensingComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Enemy FSM
	enemyFSM=CreateDefaultSubobject<UEnemyFSM>(TEXT("enemyFSM"));

	PawnSensingComponent=CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	
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

void AEnemy::DropMagazine()
{
	auto randIndex = FMath::RandRange(0, 3);
	if(randIndex==0)
	{
		auto RifleMagActor = GetWorld()->SpawnActor<ARifleMagActor>(RifleMagActorFactory, GetActorLocation(), GetActorRotation());
		if(RifleMagActor)
		{
			FVector loc = RifleMagActor->GetActorUpVector();
			RifleMagActor->magMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
	else if(randIndex==1)
	{
		auto SniperMagActor = GetWorld()->SpawnActor<ASniperMagActor>(SniperMagActorFactory, GetActorLocation(), GetActorRotation());
		if(SniperMagActor)
		{
			FVector loc = SniperMagActor->GetActorUpVector();
			SniperMagActor->magMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
	else if(randIndex==2)
	{
		auto PistolMagActor = GetWorld()->SpawnActor<APistolMagActor>(PistolMagActorFactory, GetActorLocation(), GetActorRotation());
		if(PistolMagActor)
		{
			FVector loc = PistolMagActor->GetActorUpVector();
			PistolMagActor->magMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
	else if(randIndex==3)
	{
		auto M249MagActor = GetWorld()->SpawnActor<AM249MagActor>(M249MagActorFactory, GetActorLocation(), GetActorRotation());
		if(M249MagActor)
		{
			FVector loc = M249MagActor->GetActorUpVector();
			M249MagActor->magMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
}

void AEnemy::DropGear()
{
	auto randIndex = FMath::RandRange(0, 4);
	if(randIndex==0)
	{
		auto HelmetActor = GetWorld()->SpawnActor<AHelmetActor>(HelmetActorFactory, GetActorLocation(), GetActorRotation());
		if(HelmetActor)
		{
			FVector loc = HelmetActor->GetActorUpVector();
			HelmetActor->gearMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
	else if(randIndex==1)
	{
		auto HeadsetActor = GetWorld()->SpawnActor<AHeadsetActor>(HeadsetActorFactory, GetActorLocation(), GetActorRotation());
		if(HeadsetActor)
		{
			FVector loc = HeadsetActor->GetActorUpVector();
			HeadsetActor->gearMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
	else if(randIndex==2)
	{
		auto MaskActor = GetWorld()->SpawnActor<AMaskActor>(MaskActorFactory, GetActorLocation(), GetActorRotation());
		if(MaskActor)
		{
			FVector loc = MaskActor->GetActorUpVector();
			MaskActor->gearMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
	else if(randIndex==3)
	{
		auto GoggleActor = GetWorld()->SpawnActor<AGoggleActor>(GoggleActorFactory, GetActorLocation(), GetActorRotation());
		if(GoggleActor)
		{
			FVector loc = GoggleActor->GetActorUpVector();
			GoggleActor->gearMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
	else if(randIndex==4)
	{
		auto ArmorActor = GetWorld()->SpawnActor<AArmorActor>(ArmorActorFactory, GetActorLocation(), GetActorRotation());
		if(ArmorActor)
		{
			FVector loc = ArmorActor->GetActorUpVector();
			ArmorActor->gearMesh->AddImpulseAtLocation(DropForce, loc);
		}
	}
}

void AEnemy::EnemyAttackProcess()
{
	auto particleTrans=GetMesh()->GetSocketTransform(FName("Muzzle"));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireParticle, particleTrans);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), GuardianFireSound, this->GetActorLocation());

}

