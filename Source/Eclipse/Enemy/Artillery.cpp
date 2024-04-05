// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Artillery.h"

AArtillery::AArtillery()
{
	LauncherComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LauncherComp"));
	LauncherComp->SetupAttachment(GetMesh(), FName("hand_r"));
}
