// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Eclipse/GameData/PlayerInventoryStruct.h"
#include "GameFramework/Actor.h"
#include "PickableActor.generated.h"

UCLASS()
class ECLIPSE_API APickableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickableActor();
	
	UFUNCTION()
	virtual void AddToInventory(APlayerCharacter* PlayerCharacter) const;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* RootMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Inventory)
	FPlayerInventoryStruct InventoryItemStruct;

	UPROPERTY()
	bool IsAlreadyLooted = false;

};
