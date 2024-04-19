// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Artillery.h"

#include "GuardianProjectile.h"
#include "Eclipse/AI/EnemyFSM.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AArtillery::AArtillery()
{
	LauncherComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LauncherComp"));
	LauncherComp->SetupAttachment(GetMesh(), FName("hand_r"));
}

void AArtillery::BeginPlay()
{
	Super::BeginPlay();

	const auto Material = LauncherComp->GetMaterial(0);
	if(UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this))
	{
		LauncherComp->SetMaterial(0, DynamicMaterial);
	}
}

void AArtillery::SetDissolveValue(const float Value)
{
	Super::SetDissolveValue(Value);
	const double Lerp = UKismetMathLibrary::Lerp(0, 1, Value);
	UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(LauncherComp->GetMaterial(0));
	DynamicMaterial->SetScalarParameterValue("DissolveParams", Lerp);
}

void AArtillery::FireProcess() const
{
	Super::FireProcess();
	if (EnemyFSM->player)
	{
		const FTransform MuzzleTrans = GetMesh()->GetSocketTransform(FName("ArtilleryMuzzle"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireParticle, MuzzleTrans);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GuardianFireSound, this->GetActorLocation());
		const FVector PlayerLoc = (EnemyFSM->player->GetActorLocation() - MuzzleTrans.GetLocation());
		const FRotator ProjectileRot = UKismetMathLibrary::MakeRotFromXZ(PlayerLoc, this->GetActorUpVector());
		GetWorld()->SpawnActor<AGuardianProjectile>(GuardianProjectileFactory, MuzzleTrans.GetLocation(), ProjectileRot);
	}
	
}
