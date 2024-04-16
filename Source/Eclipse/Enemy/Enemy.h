// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
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

	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY()
	class AEclipseGameMode* gameMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=FSM)
	class UEnemyFSM* EnemyFSM;

	UPROPERTY()
	class UEnemyAnim* EnemyAnim;

	UPROPERTY(EditAnywhere)
	class UPawnSensingComponent* PawnSensingComponent;

	UFUNCTION()
	void OnDie();

	UFUNCTION()
	void OnPawnDetected(APawn* Pawn); 

	UFUNCTION()
	void Damaged(int Damage, AActor* DamageCauser);

	UFUNCTION(Reliable, Server, WithValidation)
	void DamagedRPCServer(int Damage, AActor* DamageCauser);

	UFUNCTION(Unreliable, NetMulticast)
	void DamagedRPCMulticast(int Damage, AActor* DamageCauser);

	UFUNCTION()
	void OnShieldDestroy();

	UFUNCTION()
	void OnDestroy();

	UFUNCTION()
	virtual void DropReward();

	UFUNCTION()
	void DropMagazine() const;

	UFUNCTION()
	void DropGear() const;

	UFUNCTION()
	void GuardianFireProcess() const;

	UFUNCTION()
	void SetDissolveValue(float Value);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ARifleMagActor> RifleMagActorFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASniperMagActor> SniperMagActorFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APistolMagActor> PistolMagActorFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AM249MagActor> M249MagActorFactory;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHelmetActor> HelmetActorFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGoggleActor> GoggleActorFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMaskActor> MaskActorFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AArmorActor> ArmorActorFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHeadsetActor> HeadsetActorFactory;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGuardianProjectile> GuardianProjectileFactory;

	// Stat Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyCharacterStatComponent> EnemyStat;

	UPROPERTY(EditAnywhere, Category=EnemySettings)
	UAnimMontage* damageMontage;

	UPROPERTY(EditAnywhere, Category=EnemySettings)
	UAnimMontage* stunMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=EnemySettings)
	class UMaterialInterface* HitOverlayMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=EnemySettings)
	class UMaterialInterface* HitOverlayMatShield;

	UPROPERTY()
	class UEnemyHPWidget* enemyHPWidget;

	UPROPERTY()
	FTimerHandle StunHandle;

	UPROPERTY()
	FTimerHandle HPWidgetInvisibleHandle;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* fireParticle;

	UPROPERTY()
	class AEclipsePlayerController* PC;

	UPROPERTY(EditAnywhere)
	class USoundBase* GuardianFireSound;

	UPROPERTY()
	FVector DropForce = FVector(50);

	UPROPERTY()
	FVector DropLoc = GetActorUpVector();

	TArray<uint32> DynamicMaterialIndices;

	UPROPERTY()
	bool bDeath = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayerInSight = false;

	UPROPERTY(EditAnywhere) // Timeline 생성
	FTimeline DissolveTimeline;

	UPROPERTY(EditAnywhere) // Timeline 커브
	UCurveFloat* DissolveCurveFloat;
};
