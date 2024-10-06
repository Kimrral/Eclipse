// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyStatControllerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECLIPSE_API UEnemyStatControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyStatControllerComponent();

	UFUNCTION()
	void AddBossHpWidgetToViewport();
	
	UFUNCTION()
	void RemoveBossHpWidgetFromViewport() const;

	UFUNCTION()
	void UpdateBossHpWidget(const float InCurrentHp, const float InMaxHp);

	UFUNCTION()
	void UpdateBossShieldWidget(const float InCurrentShield, const float InMaxShield);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class UBossHPWidget> BossHPWidgetFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TObjectPtr<class UBossHPWidget> BossHPWidget;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
