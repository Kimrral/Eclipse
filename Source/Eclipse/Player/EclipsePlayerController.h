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
	
	UFUNCTION()
	void UpdateTabWidget() const;

	UFUNCTION()
	void UpdateBossHpWidget(const float InCurrentHp, const float InMaxHp);

	UFUNCTION()
	void UpdateBossShieldWidget(const float InCurrentShield, const float InMaxShield);

	UFUNCTION()
	void PlayerDeath() const;

	UFUNCTION()
	void AddBossHpWidgetToViewport();
	UFUNCTION()
	void RemoveBossHpWidgetFromViewport() const;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayerStart> PlayerStartFactory;	

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class UBossHPWidget> BossHPWidgetFactory;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TObjectPtr<class UBossHPWidget> BossHPWidget;

protected:
	virtual void BeginPlay() override;
};
