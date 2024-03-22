// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GuardianProjectile.generated.h"

UCLASS()
class ECLIPSE_API AGuardianProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGuardianProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* rocketMesh;
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) ;

	UFUNCTION()
	void Explosion();
	
	UPROPERTY(EditAnywhere)
	class USoundBase* explosionSound;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* explosionParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 guardianDamageValue = 30;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
