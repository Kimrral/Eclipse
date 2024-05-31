// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Shadow.h"

#include "Eclipse/AI/EnemyFSM.h"
#include "Kismet/KismetMathLibrary.h"

AShadow::AShadow()
{
	// Enemy FSM
	EnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("EnemyFSM"));
	
	WeaponComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponComp"));
	WeaponComp->SetupAttachment(GetMesh(), FName("hand_r"));
}

void AShadow::BeginPlay()
{
	Super::BeginPlay();

	const auto Material = WeaponComp->GetMaterial(0);
	if(UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this))
	{
		WeaponComp->SetMaterial(0, DynamicMaterial);
	}
}

void AShadow::SetDissolveValue(const float Value)
{
	Super::SetDissolveValue(Value);
	const double Lerp = UKismetMathLibrary::Lerp(0, 1, Value);
	UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(WeaponComp->GetMaterial(0));
	DynamicMaterial->SetScalarParameterValue("DissolveParams", Lerp);
}
