// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Game/EclipsePlayerState.h"

void AEclipsePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	UE_LOG(LogTemp, Warning, TEXT("CopyProperties C++!"))
}
