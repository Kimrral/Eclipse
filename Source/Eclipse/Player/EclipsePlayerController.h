// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "EclipsePlayerController.generated.h"

/**
 * 
 */

UCLASS()
class ECLIPSE_API AEclipsePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEclipsePlayerController();
	
	// Inventory Controller Actor Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = InventoryWidget, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInventoryControllerComponent> InventoryController;

	// Inventory Controller Actor Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyStatWidget, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyStatControllerComponent> EnemyStatController;

	// Information Widget Controller Actor Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerStatWidget, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPlayerStatControllerComponent> PlayerStatController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayerStart> PlayerStartFactory;

	UPROPERTY()
	class UInventoryWidget* InventoryWidget;

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;


protected:
	virtual void BeginPlay() override;
};
