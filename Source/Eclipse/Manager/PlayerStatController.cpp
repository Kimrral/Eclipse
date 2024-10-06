// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatController.h"

#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Player/EclipsePlayerController.h"

// Sets default values for this component's properties
UPlayerStatController::UPlayerStatController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}



// Called when the game starts
void UPlayerStatController::BeginPlay()
{
	Super::BeginPlay();

	OwningController = Cast<AEclipsePlayerController>(GetOwner());
	if (::IsValid(OwningController->PlayerCharacter))
	{
		OwningController->PlayerCharacter->Stat->OnHpChanged.AddUObject(this, &UPlayerStatController::UpdateTabWidget);
		OwningController->PlayerCharacter->Stat->OnHpZero.AddUObject(this, &UPlayerStatController::PlayerDeath);
	}	
}


// Called every frame
void UPlayerStatController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UPlayerStatController::UpdateTabWidget() const
{
	if (OwningController->PlayerCharacter)
	{
		OwningController->PlayerCharacter->UpdateTabWidgetHP();
	}
}


void UPlayerStatController::PlayerDeath() const
{
	if (OwningController->PlayerCharacter)
	{
		OwningController->PlayerCharacter->PlayerDeathRPCServer();
	}
}