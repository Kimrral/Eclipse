// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "MotionWarpingComponent.h"
#include "Components/WidgetComponent.h"
#include "Boss.generated.h"

DECLARE_DELEGATE(FOnShieldDestroySuccessed);

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
	void InitializeStat() const;
	UFUNCTION(Server, Reliable)
	void InitializeStatServer() const;

	UFUNCTION()
	void SetBossShieldWidget(const bool bEnable);

	UFUNCTION(Server, Reliable)
	void SetBossShieldWidgetServer(const bool bEnable);

	UFUNCTION(NetMulticast, Unreliable)
	void SetBossShieldWidgetMulticast(const bool bEnable);

	UFUNCTION()
	void SetBossShieldWidgetDelegate(const float InCurShield, const float InMaxShield) const;

	UFUNCTION()
	void LaunchBossCharacter(const FVector& TargetLocation) const;

	UFUNCTION()
	void PlayAnimMontageBySectionName(const FName& SectionName);
	UFUNCTION(Server, Reliable)
	void PlayAnimMontageBySectionNameServer(const FName& SectionName);
	UFUNCTION(NetMulticast, Unreliable)
	void PlayAnimMontageBySectionNameMulticast(const FName& SectionName);

	UPROPERTY(EditAnywhere, Category=Variable)
	float DashForce;

	UPROPERTY(EditAnywhere, Category=Widget)
	TObjectPtr<class UWidgetComponent> ShieldWidgetComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	FOnShieldDestroySuccessed ShieldDestroySuccessDelegate;

	UPROPERTY()
	class UBossShieldWidget* BossShieldWidget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
