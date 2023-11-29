// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponActor.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	weaponMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("weaponMesh"));
	SetRootComponent(weaponMesh);

	weaponMesh->SetGenerateOverlapEvents(true);

	weaponMesh->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::OnOverlap);
	weaponMesh->OnComponentEndOverlap.AddDynamic(this, &AWeaponActor::EndOverlap);

}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
	weaponMesh->SetRenderCustomDepth(false);
	
}

// Called every frame
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AWeaponActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

