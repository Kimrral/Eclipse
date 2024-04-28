// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Eclipse/GameData/PlayerInventoryStruct.h"
#include "GameFramework/Actor.h"
#include "DeadPlayerContainer.generated.h"

UCLASS()
class ECLIPSE_API ADeadPlayerContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeadPlayerContainer();

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* DeadBodyMesh;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	TArray<FPlayerInventoryStruct> DeadPlayerInventoryStructArray;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	TArray<int32> DeadPlayerInventoryStackArray;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	TArray<FPlayerInventoryStruct> DeadPlayerGearSlotArray;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


};
