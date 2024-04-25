// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeadPlayerContainer.generated.h"

UCLASS()
class ECLIPSE_API ADeadPlayerContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeadPlayerContainer();
	
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* DeadBodyMesh;

};
