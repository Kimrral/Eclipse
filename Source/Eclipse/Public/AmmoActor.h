// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InformationWidget.h"
#include "GameFramework/Actor.h"
#include "AmmoActor.generated.h"

UCLASS()
class ECLIPSE_API AAmmoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ammoMesh;

	UPROPERTY(EditAnywhere)
	class USphereComponent* playerTraceSphereCollision;

	UPROPERTY(EditAnywhere)
	class USoundBase* ammoPickupSound;

};
