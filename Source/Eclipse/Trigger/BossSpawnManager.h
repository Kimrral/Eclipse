// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossSpawnManager.generated.h"

UCLASS()
class ECLIPSE_API ABossSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossSpawnManager();

	UFUNCTION()
	void ActivateBossSpawnTimer();

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	class USceneComponent* RootScene;
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* SpawnTriggerBoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class ABoss> BossFactory;

	UPROPERTY()
	class ABoss* SpawnedBoss;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCoolTime = 60.f;

	UPROPERTY()
	bool IsSpawnable = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
