// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnManager.generated.h"

UCLASS()
class ECLIPSE_API AEnemySpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnManager();

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere)
	class UBoxComponent* SpawnTriggerBoxCollision;

	UPROPERTY(EditAnywhere)
	class USceneComponent* SpawnPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class AEnemy> EnemyFactory;

	UPROPERTY()
	bool IsSpawnable = true;

	UPROPERTY()
	float SpawnCoolTime = 15.f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
