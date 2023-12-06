// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoActor.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class ECLIPSE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY()
	class AEclipseGameMode* gameMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FSM)
	class UEnemyFSM* enemyFSM;

	UPROPERTY()
	class UEnemyAnim* enemyAnim;

	UFUNCTION()
	void Move();

	UFUNCTION()
	void OnDie();
	
	UFUNCTION()
	void OnDamaged();

	UFUNCTION()
	void OnHeadDamaged();

	UFUNCTION()
	void OnDestroy();

	UFUNCTION()
	void SetHPWidgetInvisible();

	UFUNCTION()
	void DropReward();

	UFUNCTION()
	void DetectPlayerLineTrace();

	UFUNCTION()
	void EnemyAttackProcess();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APistolAmmoActor> pistolAmmoFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ARifleAmmoActor> rifleAmmoFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASniperAmmoActor> sniperAmmoFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AM249AmmoActor> M249AmmoFactory;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int curHP;
	
	UPROPERTY(VisibleAnywhere, Category=EnemySettings)
	int maxHP = 100.0f;

	UPROPERTY(EditAnywhere, Category=EnemySettings)
	UAnimMontage* damageMontage;

	UPROPERTY(EditAnywhere, Category=EnemySettings)
	class UWidgetComponent* HPWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category=EnemySettings)
	class UMaterialInterface* overlayMatRed;

	UPROPERTY()
	class UEnemyHPWidget* enemyHPWidget;

	UPROPERTY(EditAnywhere, Category=EnemySettings)
	class UStaticMeshComponent* aimingPointer;

	UPROPERTY()
	FTimerHandle HPWidgetInvisibleHandle;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* fireParticle;

	UPROPERTY()
	class AEclipsePlayerController* PC;

	UPROPERTY(EditAnywhere, Category=EnemySettings)
	class UMaterialInterface* M_aimingPointer;

	UPROPERTY(EditAnywhere, Category=EnemySettings)
	class UMaterialInterface* M_aimingPointerHead;

	UPROPERTY()
	bool bDeath = false;

	UPROPERTY()
	bool bPlayerInSight = false;

};
