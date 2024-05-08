// Fill out your copyright notice in the Description page of Project Settings.


#include "Trooper.h"

#include "TrooperProjectile.h"
#include "Eclipse/AI/EnemyFSM.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

ATrooper::ATrooper()
{
	WeaponComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponComp"));
	WeaponComp->SetupAttachment(GetMesh(), FName("hand_r"));
}

void ATrooper::BeginPlay()
{
	Super::BeginPlay();

	const auto Material = WeaponComp->GetMaterial(0);
	if(UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this))
	{
		WeaponComp->SetMaterial(0, DynamicMaterial);
	}
}

void ATrooper::SetDissolveValue(const float Value)
{
	Super::SetDissolveValue(Value);
	const double Lerp = UKismetMathLibrary::Lerp(0, 1, Value);
	UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(WeaponComp->GetMaterial(0));
	DynamicMaterial->SetScalarParameterValue("DissolveParams", Lerp);
}

void ATrooper::FireProcess() const
{
	if (EnemyFSM->player)
	{
		const FTransform MuzzleTrans = GetMesh()->GetSocketTransform(FName("TrooperMuzzle"));
		const FVector PlayerLoc = (EnemyFSM->player->GetActorLocation() - MuzzleTrans.GetLocation());
		const FRotator ProjectileRot = UKismetMathLibrary::MakeRotFromXZ(PlayerLoc, this->GetActorUpVector());		
		const auto ProjectileTrans = UKismetMathLibrary::MakeTransform(MuzzleTrans.GetLocation(), ProjectileRot+FRotator(0, -90, 0));
		GetWorld()->SpawnActor<ATrooperProjectile>(TrooperProjectileFactory, ProjectileTrans);
	}
}
