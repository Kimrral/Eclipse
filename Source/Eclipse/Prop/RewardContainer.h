// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RewardContainer.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FRewardContainerDestruct, FVector, ContainerLocation);

UCLASS()
class ECLIPSE_API ARewardContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARewardContainer();
	
	UFUNCTION()
	void DropReward() const;

	UFUNCTION()
	void DropConsole() const;

	UFUNCTION()
	void DropMagazine() const;

	UFUNCTION()
	void DropConsumable() const;

	UFUNCTION()
	void DropETC() const;

	UFUNCTION()
	void BoxDestroyed();

	UFUNCTION()
	void OnRep_IsBoxDestroyed();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UGeometryCollectionComponent* containerMesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHackingConsole> HackingConsoleFactory;	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ARifleMagActor> RifleMagActorFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASniperMagActor> SniperMagActorFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APistolMagActor> PistolMagActorFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AM249MagActor> M249MagActorFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMilitaryDevice> MilitaryDeviceFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMilitaryLaptop> MilitaryLaptopFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMedKitActor> MedKitActorFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AAdrenalineSyringe> AdrenalineSyringeFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AFirstAidKitActor> FirstAidKitActorFactory;
	
	UPROPERTY(EditAnywhere)
	class USoundBase* containerBreakSound;
	
	UPROPERTY()
	FVector DropForce;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRewardContainerDestruct containerDele;
	
	UPROPERTY()
	int curBoxHP;

	UPROPERTY()
	int maxBoxHP = 5;

	UPROPERTY(ReplicatedUsing=OnRep_IsBoxDestroyed)
	bool IsBoxDestroyed = false;	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
