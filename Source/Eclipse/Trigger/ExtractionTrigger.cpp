// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Trigger/ExtractionTrigger.h"

#include "Components/BoxComponent.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/UI/ExtractionCountdown.h"

// Sets default values
AExtractionTrigger::AExtractionTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);
	BoxCollision->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AExtractionTrigger::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AExtractionTrigger::OnOverlap);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AExtractionTrigger::EndOverlap);
}

// Called every frame
void AExtractionTrigger::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AExtractionTrigger::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (const auto Player = Cast<APlayerCharacter>(OtherActor); Player&&Player->IsLocallyControlled() && !Player->ExtractionCountdownUI->IsInViewport())
		{
			Player->ExtractionCountdownUI->Countdown=10.f;
			Player->ExtractionCountdownUI->AddToViewport();
		}
	}
}

void AExtractionTrigger::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (const auto Player = Cast<APlayerCharacter>(OtherActor); Player&&Player->IsLocallyControlled() && Player->ExtractionCountdownUI->IsInViewport())
		{
			Player->ExtractionCountdownUI->RemoveFromParent();
		}
	}
}
