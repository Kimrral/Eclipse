// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RewardContainer.generated.h"

UCLASS()
class ECLIPSE_API ARewardContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARewardContainer();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UGeometryCollectionComponent* containerMesh;

	UPROPERTY(EditAnywhere)
	class USphereComponent* playerDetectCollision;

	UFUNCTION()
	void SphereOnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) ;
	UFUNCTION()
	void SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
	UFUNCTION()
	void DropReward();

	UFUNCTION()
	void DropAmmo();

	UFUNCTION()
	void BoxDestroyed();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APistolAmmoActor> pistolAmmoFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ARifleAmmoActor> rifleAmmoFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASniperAmmoActor> sniperAmmoFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AM249AmmoActor> M249AmmoFactory;	

	UPROPERTY()
	int curBoxHP;

	UPROPERTY()
	int maxBoxHP = 10;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
