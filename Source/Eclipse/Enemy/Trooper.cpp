// Fill out your copyright notice in the Description page of Project Settings.


#include "Trooper.h"

#include "TrooperProjectile.h"
#include "Eclipse/AI/EnemyFSM.h"
#include "Eclipse/Item/RewardManagerComponent.h"
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
	if (UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this))
	{
		WeaponComp->SetMaterial(0, DynamicMaterial);
	}
}

void ATrooper::OnDestroy()
{
	if(HasAuthority())
	{		
		RewardManager->DropSniperServer(GetActorTransform());
	}
	DissolveTimeline.PlayFromStart();
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
	if (EnemyFSM->Player)
	{
		const FVector MuzzleLocation = WeaponComp->GetSocketLocation(FName("Muzzle"));
		const FVector TargetLocation = EnemyFSM->Player->GetMesh()->GetBoneLocation(FName("head"));
		UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), TargetLocation.X, TargetLocation.Y, TargetLocation.Z)
		const FRotator ProjectileRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation) + FRotator(0, -90, 0);
		GetWorld()->SpawnActor<ATrooperProjectile>(TrooperProjectileFactory, MuzzleLocation, ProjectileRot);
	}
}
