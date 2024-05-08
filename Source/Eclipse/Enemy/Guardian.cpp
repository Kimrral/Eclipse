// Fill out your copyright notice in the Description page of Project Settings.


#include "Guardian.h"

#include "GuardianProjectile.h"
#include "Eclipse/AI/EnemyFSM.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void AGuardian::DropRewardServer()
{
	Super::DropRewardServer();
}

void AGuardian::FireProcess() const
{
	if (EnemyFSM->player)
	{
		const FTransform MuzzleTrans = GetMesh()->GetSocketTransform(FName("Muzzle"));
		const FVector PlayerLoc = (EnemyFSM->player->GetActorLocation() - MuzzleTrans.GetLocation());
		const FRotator ProjectileRot = UKismetMathLibrary::MakeRotFromXZ(PlayerLoc, this->GetActorUpVector());
		GetWorld()->SpawnActor<AGuardianProjectile>(GuardianProjectileFactory, MuzzleTrans.GetLocation(), ProjectileRot);
	}
}
