// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoActor.h"
#include "SniperAmmoActor.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API ASniperAmmoActor : public AAmmoActor
{
	GENERATED_BODY()


	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) ;

	UFUNCTION()
	void SphereOnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) ;

	UPROPERTY()
	float moveSpeed = 500.f;

	UPROPERTY()
	FVector direction;

	UPROPERTY()
	bool bTracePlayer = false;
	
};
