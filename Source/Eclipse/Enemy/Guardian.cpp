// Fill out your copyright notice in the Description page of Project Settings.


#include "Guardian.h"

#include "GuardianProjectile.h"
#include "Eclipse/AI/EnemyFSM.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/Item/RewardManagerComponent.h"
#include "Kismet/KismetMathLibrary.h"


AGuardian::AGuardian()
{
	// Enemy FSM
	EnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("EnemyFSM"));
}

void AGuardian::OnDestroy()
{
	if (HasAuthority())
	{
		RewardManager->DropRewardServer(GetActorTransform());
		SetLifeSpan(30.f);
	}
}

void AGuardian::FireProcess() const
{
	if (EnemyFSM->Player)
	{
		const FTransform MuzzleTrans = GetMesh()->GetSocketTransform(FName("Muzzle"));
		const FVector PlayerLoc = (EnemyFSM->Player->GetActorLocation() - MuzzleTrans.GetLocation());
		const FRotator ProjectileRot = UKismetMathLibrary::MakeRotFromXZ(PlayerLoc, this->GetActorUpVector());
		GetWorld()->SpawnActor<AGuardianProjectile>(GuardianProjectileFactory, MuzzleTrans.GetLocation(), ProjectileRot);
	}
}

void AGuardian::SetDissolveMaterial()
{
	return;
}

void AGuardian::SetDissolveValue(float Value)
{
	return;
}
