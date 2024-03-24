// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageWidgetActor.h"

#include "Components/WidgetComponent.h"

// Sets default values
ADamageWidgetActor::ADamageWidgetActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DamageWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidgetComponent"));
	SetRootComponent(DamageWidgetComponent);
}

// Called when the game starts or when spawned
void ADamageWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(1.5f);
}

// Called every frame
void ADamageWidgetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
