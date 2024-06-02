// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Boss.generated.h"

UCLASS()
class ECLIPSE_API ABoss : public AEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoss();

	virtual void OnDie() override;
	virtual void OnDestroy() override;
	virtual void OnShieldDestroy() override;
	
	virtual void SetAIController() override;
	virtual void SetDissolveMaterial() override;	
	virtual void SetDissolveValue(float Value) override;

	UFUNCTION()
	void LaunchBossCharacter();

	UFUNCTION()
	void PlayAnimMontageBySectionName(const FName& SectionName);
	UFUNCTION(Server, Reliable)
	void PlayAnimMontageBySectionNameServer(const FName& SectionName);
	UFUNCTION(NetMulticast, Unreliable)
	void PlayAnimMontageBySectionNameMulticast(const FName& SectionName);
	
	UPROPERTY(EditAnywhere, Category=Variable)
	float DashForce;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



};
