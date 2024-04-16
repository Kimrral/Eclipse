// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Artillery.h"

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
