// Fill out your copyright notice in the Description page of Project Settings.


#include "BossHPWidgetTrigger.h"

#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
ABossHPWidgetTrigger::ABossHPWidgetTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxCollision=CreateDefaultSubobject<UBoxComponent>(TEXT("boxCollision"));
	SetRootComponent(boxCollision);
	boxCollision->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ABossHPWidgetTrigger::BeginPlay()
{
	Super::BeginPlay();

	boxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossHPWidgetTrigger::OnOverlap);
	
}

// Called every frame
void ABossHPWidgetTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossHPWidgetTrigger::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		auto player=Cast<APlayerCharacter>(OtherActor);
		if(player)
		{
			if(!player->bossHPUI->IsInViewport())
			{
				player->bossHPUI->AddToViewport();
				this->Destroy();
			}
		}
	}
}

