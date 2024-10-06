// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Manager/PlayerStatControllerComponent.h"

#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Player/EclipsePlayerController.h"

// Sets default values for this component's properties
UPlayerStatControllerComponent::UPlayerStatControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}



// Called when the game starts
void UPlayerStatControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningController = Cast<AEclipsePlayerController>(GetOwner());
	if (::IsValid(OwningController->PlayerCharacter))
	{
		OwningController->PlayerCharacter->Stat->OnHpChanged.AddUObject(this, &UPlayerStatControllerComponent::UpdateTabWidget);
		OwningController->PlayerCharacter->Stat->OnHpZero.AddUObject(this, &UPlayerStatControllerComponent::PlayerDeath);
	}	
}


// Called every frame
void UPlayerStatControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UPlayerStatControllerComponent::UpdateTabWidget() const
{
	if (OwningController->PlayerCharacter)
	{
		OwningController->PlayerCharacter->UpdateTabWidgetHP();
	}
}


void UPlayerStatControllerComponent::PlayerDeath() const
{
	if (OwningController->PlayerCharacter)
	{
		OwningController->PlayerCharacter->PlayerDeathRPCServer();
	}
}