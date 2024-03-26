// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Game/EclipsePlayerState.h"


void AEclipsePlayerState::BeginPlay()
{
	Super::BeginPlay();

	PlayerUniqueNetId = BP_GetUniqueId();
}
