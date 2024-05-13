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
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Sets default values for this actor's properties
	ARewardContainer();

	UFUNCTION()
	void BoxDestroyed();

	UFUNCTION()
	void OnRep_IsBoxDestroyed();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Reward Manager
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class URewardManagerComponent> RewardManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UGeometryCollectionComponent* ContainerMesh;
	
	UPROPERTY(EditAnywhere)
	class USoundBase* ContainerBreakSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRewardContainerDestruct containerDele;
	
	UPROPERTY()
	int CurBoxHP;

	UPROPERTY()
	int MaxBoxHP = 5;

	UPROPERTY(ReplicatedUsing=OnRep_IsBoxDestroyed)
	bool IsBoxDestroyed = false;	


};
