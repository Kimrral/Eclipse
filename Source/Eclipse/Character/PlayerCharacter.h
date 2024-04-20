// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Eclipse/UI/BossHPWidget.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Eclipse/Weapon/RifleActor.h"
#include "Eclipse/UI/StashWidget.h"
#include "Components/TimelineComponent.h"
#include "Eclipse/Game/EclipseGameInstance.h"
#include "Eclipse/Item/ArmorActor.h"
#include "Eclipse/Prop/RewardContainer.h"
#include "Eclipse/Weapon/M249Actor.h"
#include "Eclipse/Weapon/PistolActor.h"
#include "Eclipse/Weapon/SniperActor.h"
#include "PlayerCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnWeaponChange);
DECLARE_DYNAMIC_DELEGATE(FOnDoorInteraction);

UCLASS()
class ECLIPSE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** First Person Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FirstPersonCharacterMesh;

	/** First Person Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCamera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Stat Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPlayerCharacterStatComponent> Stat;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	/** Zoom Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ZoomAction;

	/** Run Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RunAction;

	/** Crouch Action Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	/** Change Weapon Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponAction;

	/** Reload Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAroundAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ZoomInAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ZoomOutAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FirstWeaponSwapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SecondWeaponSwapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TabAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* QAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnDoorInteraction DoorInteractionDele;

	FOnWeaponChange WeaponChangeDele;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void Jump() override;
	virtual void StopJumping() override;

	//=======================================//

	/** Called for zooming input */
	void Zoom(const bool IsZoomInput);
	void ZoomRelease(const bool IsZoomInput);
	void ZoomInput();
	void ZoomReleaseInput();

	UFUNCTION(Reliable, Server, WithValidation)
	void ZoomRPCServer(const bool IsZoomInput);

	UFUNCTION(Unreliable, NetMulticast)
	void ZoomRPCMulticast(const bool IsZoomInput);

	UFUNCTION(Reliable, Server, WithValidation)
	void ZoomRPCReleaseServer(const bool IsZoomInput);

	UFUNCTION(Unreliable, NetMulticast)
	void ZoomRPCReleaseMulticast(const bool IsZoomInput);

	//=======================================//

	/** Called for fire input */
	UFUNCTION()
	void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCFire();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCFire();

	void FireRelease();

	UFUNCTION()
	void ProcessRifleFire();
	UFUNCTION()
	void ProcessRifleFireAnim();
	UFUNCTION()
	void ProcessRifleFireLocal();
	UFUNCTION()
	void ProcessRifleFireSimulatedProxy() const;

	UFUNCTION()
	void ProcessSniperFire();
	UFUNCTION()
	void ProcessSniperFireAnim();
	UFUNCTION()
	void ProcessSniperFireLocal();
	UFUNCTION()
	void ProcessSniperFireSimulatedProxy() const;

	UFUNCTION()
	void ProcessPistolFire();
	UFUNCTION()
	void ProcessPistolFireAnim();
	UFUNCTION()
	void ProcessPistolFireLocal();
	UFUNCTION()
	void ProcessPistolFireSimulatedProxy() const;


	UFUNCTION()
	void ProcessM249Fire();
	UFUNCTION()
	void ProcessM249FireAnim();
	UFUNCTION()
	void ProcessM249FireLocal();
	UFUNCTION()
	void ProcessM249FireSimulatedProxy() const;

	UFUNCTION()
	float SetFireInterval();

	UFUNCTION()
	float GetAttackDamage(bool IsPlayer);
	UFUNCTION()
	int32 GetAttackDamageCache(bool IsPlayer);

	UFUNCTION()
	void ChangeWeaponToRifle(ARifleActor* RifleActor);
	UFUNCTION(Server, Reliable, WithValidation)
	void ChangeWeaponToRifleRPCServer(ARifleActor* RifleActor);
	UFUNCTION(NetMulticast, UnReliable)
	void ChangeWeaponToRifleRPCMulticast(ARifleActor* RifleActor);

	UFUNCTION()
	void ChangeWeaponToSniper(ASniperActor* SniperActor);
	UFUNCTION(Server, Reliable, WithValidation)
	void ChangeWeaponToSniperRPCServer(ASniperActor* SniperActor);
	UFUNCTION(NetMulticast, UnReliable)
	void ChangeWeaponToSniperRPCMulticast(ASniperActor* SniperActor);


	UFUNCTION()
	void ChangeWeaponToPistol(APistolActor* PistolActor);
	UFUNCTION(Server, Reliable, WithValidation)
	void ChangeWeaponToPistolRPCServer(APistolActor* PistolActor);
	UFUNCTION(NetMulticast, UnReliable)
	void ChangeWeaponToPistolRPCMulticast(APistolActor* PistolActor);


	UFUNCTION()
	void ChangeWeaponToM249(AM249Actor* M249Actor);
	UFUNCTION(Server, Reliable, WithValidation)
	void ChangeWeaponToM249RPCServer(AM249Actor* M249Actor);
	UFUNCTION(NetMulticast, UnReliable)
	void ChangeWeaponToM249RPCMulticast(AM249Actor* M249Actor);

	//=======================================//

	/** Called for running input */
	UFUNCTION()
	void Run();
	UFUNCTION()
	void RunRelease();

	UFUNCTION(Reliable, Server, WithValidation)
	void RunRPCServer();

	UFUNCTION(Unreliable, NetMulticast)
	void RunRPCMulticast();

	UFUNCTION(Reliable, Server, WithValidation)
	void RunRPCReleaseServer();

	UFUNCTION(Unreliable, NetMulticast)
	void RunRPCReleaseMulticast();

	//=======================================//

	/** Called for zooming input */
	void Crouching();

	//=======================================//

	/** Called for change weapon input */
	UFUNCTION()
	void ChangeWeapon();

	UFUNCTION()
	void PickableItemActorInteraction(APickableActor* PickableActor);
	UFUNCTION(Reliable, Server, WithValidation)
	void PickableItemActorInteractionRPCServer(APickableActor* PickableActor);
	UFUNCTION(Unreliable, NetMulticast)
	void PickableItemActorInteractionRPCMutlicast(APickableActor* PickableActor);

	UFUNCTION()
	void DeadBodyInteraction(APlayerCharacter* DeadPlayerCharacter);
	UFUNCTION(Reliable, Server, WithValidation)
	void DeadBodyInteractionRPCServer(APlayerCharacter* DeadPlayerCharacter);
	UFUNCTION(Unreliable, NetMulticast)
	void DeadBodyInteractionRPCMutlicast(APlayerCharacter* DeadPlayerCharacter);


	//=======================================//

	UFUNCTION()
	void Reload();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCReload();

	UFUNCTION(NetMulticast, UnReliable)
	void MulticastRPCReload();

	//=======================================//

	void OnActionLookAroundPressed();
	void OnActionLookAroundReleased();

	//=======================================//

	UFUNCTION()
	void SwapFirstWeapon();

	UFUNCTION(Reliable, Server, WithValidation)
	void SwapFirstWeaponRPCServer();

	UFUNCTION(Unreliable, NetMulticast)
	void SwapFirstWeaponRPCMulticast();

	UFUNCTION()
	void SwapSecondWeapon();

	UFUNCTION(Reliable, Server, WithValidation)
	void SwapSecondWeaponRPCServer();

	UFUNCTION(Unreliable, NetMulticast)
	void SwapSecondWeaponRPCMulticast();

	//=======================================//

	UFUNCTION()
	void PlayerDeath();

	UFUNCTION(Reliable, Server, WithValidation)
	void PlayerDeathRPCServer();

	UFUNCTION(Unreliable, NetMulticast)
	void PlayerDeathRPCMulticast();

	//=======================================//

	UFUNCTION()
	void Damaged(int Damage, AActor* DamageCauser);

	UFUNCTION(Reliable, Server, WithValidation)
	void DamagedRPCServer(int Damage, AActor* DamageCauser);

	UFUNCTION(Unreliable, NetMulticast)
	void DamagedRPCMulticast(int Damage, AActor* DamageCauser);

	//=======================================//

	UFUNCTION()
	void OnPlayerHit(const FHitResult& HitResult, APlayerCharacter* HitCharacter, bool IsHeadshot);

	UFUNCTION(Reliable, Server, WithValidation)
	void OnPlayerHitRPCServer(const FHitResult& HitResult, APlayerCharacter* HitCharacter, bool IsHeadshot);

	UFUNCTION(Unreliable, NetMulticast)
	void OnPlayerHitRPCMulticast(const FHitResult& HitResult, APlayerCharacter* HitCharacter, bool IsHeadshot);

	//=======================================//

	UFUNCTION()
	void OnPlayerKill();

	UFUNCTION(Reliable, Server, WithValidation)
	void OnPlayerKillRPCServer();

	UFUNCTION(Unreliable, NetMulticast)
	void OnPlayerKillRPCMulticast();

	//=======================================//

	UFUNCTION()
	void OnEnemyHit(const FHitResult& HitResult, AEnemy* HitEnemy, bool IsHeadshot);

	UFUNCTION(Reliable, Server, WithValidation)
	void OnEnemyHitRPCServer(const FHitResult& HitResult, AEnemy* HitEnemy, bool IsHeadshot);

	UFUNCTION(Unreliable, NetMulticast)
	void OnEnemyHitRPCMulticast(const FHitResult& HitResult, AEnemy* HitEnemy, bool IsHeadshot);

	//=======================================//

	UFUNCTION()
	void OnEnemyKill();

	UFUNCTION(Reliable, Server, WithValidation)
	void OnEnemyKillRPCServer();

	UFUNCTION(Unreliable, NetMulticast)
	void OnEnemyKillRPCMulticast();

	//=======================================//

	UFUNCTION()
	void OnGroundHit(const FHitResult& HitResult);

	UFUNCTION(Reliable, Server, WithValidation)
	void OnGroundHitRPCServer(const FHitResult& HitResult);

	UFUNCTION(Unreliable, NetMulticast)
	void OnGroundHitRPCMulticast(const FHitResult& HitResult);

	//=======================================//

	UFUNCTION()
	void OnContainerHit(const FHitResult& HitResult, ARewardContainer* HitContainer);

	UFUNCTION(Reliable, Server, WithValidation)
	void OnContainerHitRPCServer(const FHitResult& HitResult, ARewardContainer* HitContainer);

	UFUNCTION(Unreliable, NetMulticast)
	void OnContainerHitRPCMulticast(const FHitResult& HitResult, ARewardContainer* HitContainer);

	//=======================================//

	UFUNCTION()
	void DoorInteraction();

	UFUNCTION(Reliable, Server, WithValidation)
	void DoorInteractionRPCServer();

	UFUNCTION(Unreliable, NetMulticast)
	void DoorInteractionRPCMulticast();

	//=======================================//

	void Tab();

	void OpenMenu();

	void Q();

	UFUNCTION()
	void WeaponDetectionLineTrace();

	UFUNCTION()
	void SetBossHPWidget(const AEnemy* Enemy) const;

	UFUNCTION()
	void SetDamageWidget(int Damage, const FVector& SpawnLoc, bool bIsShieldIconEnable, FLinearColor DamageTextColor);

	UFUNCTION()
	void RemoveBossHPWidget() const;

	UFUNCTION()
	void InfoWidgetUpdate();

	UFUNCTION()
	float DamageMultiplier() const;

	UFUNCTION()
	float FireRateMultiplier() const;

	UFUNCTION()
	float RecoilRateMultiplier() const;

	UFUNCTION(BlueprintImplementableEvent)
	void ClearInventoryCache();

	UFUNCTION(BlueprintCallable)
	void EquipHelmet(bool SoundBool);

	UFUNCTION(BlueprintCallable)
	void EquipHeadset(bool SoundBool);

	UFUNCTION(BlueprintCallable)
	void EquipMask(bool SoundBool);

	UFUNCTION(BlueprintCallable)
	void EquipGoggle(bool SoundBool);

	UFUNCTION(BlueprintCallable)
	void UnEquipHelmet(bool SoundBool);

	UFUNCTION(BlueprintCallable)
	void UnEquipHeadset(bool SoundBool);

	UFUNCTION(BlueprintCallable)
	void UnEquipMask(bool SoundBool);

	UFUNCTION(BlueprintCallable)
	void UnEquipGoggle(bool SoundBool);

	UFUNCTION(BlueprintCallable)
	void EquipArmor(bool SoundBool);

	UFUNCTION(BlueprintCallable)
	void UnEquipArmor(bool SoundBool);

	UFUNCTION(BlueprintImplementableEvent)
	void StashWidgetOnViewport();

	UFUNCTION(BlueprintImplementableEvent)
	void DeadBodyWidgetOnViewport();

	UFUNCTION()
	void InteractionProcess();

	UFUNCTION()
	void MoveToIsolatedShip();
	
	UFUNCTION()
	void MoveToHideout();

	UFUNCTION()
	void MoveToBlockedIntersection();

	UFUNCTION() // Bind function
	void SetZoomValue(float Value);

	UFUNCTION()
	void CachingValues() const;

	UFUNCTION()
	void ApplyCachingValues();

	UFUNCTION(BlueprintImplementableEvent)
	void PouchCaching();

	UFUNCTION(BlueprintImplementableEvent)
	void StashCaching();

	UFUNCTION(BlueprintImplementableEvent)
	void GearCaching();

	UFUNCTION(BlueprintImplementableEvent)
	void MagCaching();

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyStashCache();

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyInventoryCache();

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyPouchCache();

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyGearCache();

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyMagCache();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTabWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTabWidgetHP();

	UFUNCTION()
	int32 SetRifleAdditionalMagazine() const;
	UFUNCTION()
	int32 SetSniperAdditionalMagazine() const;
	UFUNCTION()
	int32 SetPistolAdditionalMagazine() const;
	UFUNCTION()
	int32 SetM249AdditionalMagazine() const;

	UFUNCTION(BlueprintCallable)
	void SetRifleAdditionalMagazineSlot();
	UFUNCTION(BlueprintCallable)
	void SetSniperAdditionalMagazineSlot();
	UFUNCTION(BlueprintCallable)
	void SetPistolAdditionalMagazineSlot();
	UFUNCTION(BlueprintCallable)
	void SetM249AdditionalMagazineSlot();

	UFUNCTION(BlueprintCallable)
	void UnSetRifleAdditionalMagazineSlot();
	UFUNCTION(BlueprintCallable)
	void UnSetSniperAdditionalMagazineSlot();
	UFUNCTION(BlueprintCallable)
	void UnSetPistolAdditionalMagazineSlot();
	UFUNCTION(BlueprintCallable)
	void UnSetM249AdditionalMagazineSlot();

	UFUNCTION()
	void OnRep_WeaponArrayChanged() const;

	UFUNCTION()
	int32 GenerateRandomDamage(float InDamage) const;

	UFUNCTION()
	void AmmoDepleted();

	UFUNCTION()
	void ExtractionSuccess() const;

	UFUNCTION()
	void SetFirstPersonModeRifle(const bool IsFirstPerson);
	UFUNCTION()
	void SetFirstPersonModePistol(const bool IsFirstPerson);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* sniperComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* rifleComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* FirstPersonRifleComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* pistolComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* FirstPersonPistolComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* m249Comp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* grenade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* rocketLauncher;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* HeadSetSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* GoggleSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* MaskSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* HelmetSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* ArmorSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class ARifleActor> rifleFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class ASniperActor> sniperFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class APistolActor> pistolFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class AM249Actor> M249Factory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class ADamageWidgetActor> damageWidgetFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class UDamageWidget> damageWidgetUIFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class UBossHPWidget> bossHPWidgetFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class UExtractionCountdown> ExtractionCountdownWidgetFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = widget)
	class UMenuWidget* MenuWidgetUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class UMenuWidget> MenuWidgetFactory;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = widget)
	class UExtractionCountdown* ExtractionCountdownUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = widget)
	class UDamageWidget* damageWidgetUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = widget)
	class UBossHPWidget* bossHPUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class UQuitWidget> quitWidgetFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = widget)
	class UQuitWidget* quitWidgetUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class ULevelSelection> levelSelectionWidgetFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = widget)
	class ULevelSelection* levelSelectionUI;

	UPROPERTY()
	FTimerHandle ZoomFireHandle;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ShieldHitEmitter;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* PlayerSpawnEmitter;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* PlayerDeathEmitter;

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	class APickableActor* PickableItemActor;

	UPROPERTY()
	class AMaskActor* MaskActor;

	UPROPERTY()
	class AHelmetActor* HelmetActor;

	UPROPERTY()
	class AHeadsetActor* HeadsetActor;

	UPROPERTY()
	class AGoggleActor* GoggleActor;

	UPROPERTY()
	class AArmorActor* ArmorActor;

	UPROPERTY()
	class AMedKitActor* MedKitActor;

	UPROPERTY()
	class ARifleActor* rifleActor;

	UPROPERTY()
	class ASniperActor* sniperActor;

	UPROPERTY()
	class APistolActor* pistolActor;

	UPROPERTY()
	class AM249Actor* m249Actor;

	UPROPERTY()
	class ARifleActor* OverlappedRifleActor;

	UPROPERTY()
	class ASniperActor* OverlappedSniperActor;

	UPROPERTY()
	class APistolActor* OverlappedPistolActor;

	UPROPERTY()
	class UPlayerAnim* animInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	class UAnimMontage* UpperOnlyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	class UAnimMontage* FullBodyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	class UAnimMontage* RifleFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	class UAnimMontage* FirstPersonRifeZoomMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	class UAnimMontage* FirstPersonPistolZoomMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	class UAnimMontage* FirstPersonPistolFireMontage;

	UPROPERTY(EditAnywhere)
	class USphereComponent* weaponDetectionCollision;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> crosshairFactory;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> sniperScopeFactory;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UWeaponInfoWidget> infoWidgetFactory;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UInformationWidget> informationWidgetFactory;

	UPROPERTY(BlueprintReadOnly)
	class UInformationWidget* informationUI;

	UPROPERTY(BlueprintReadOnly)
	class UWeaponInfoWidget* infoWidgetUI;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> tabWidgetFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTabWidget* tabWidgetUI;

	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_WeaponArrayChanged)
	TArray<bool> weaponArray;

	UPROPERTY(EditAnywhere, Category="Texture")
	class UTexture* LoadingTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> equippedWeaponStringArray;

	UPROPERTY(BlueprintReadOnly)
	class UCrosshairWidget* crosshairUI;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* bulletImpactFactory;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* BuletTrailSystem;

	UPROPERTY(BlueprintReadOnly)
	class UUserWidget* sniperScopeUI;

	UPROPERTY()
	class AActor* hitActors;

	UPROPERTY()
	class AEnemy* enemyRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Material)
	class UMaterialInterface* overlayMatRed;

	FVector RifleLineTraceStart;

	FVector RifleLineTraceEnd;

	FVector PistolLineTraceStart;

	FVector PistolLineTraceEnd;

	UPROPERTY()
	FVector DeathPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int curWeaponSlotNumber = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUsingRifle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUsingSniper;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUsingPistol;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUsingM249;

	UPROPERTY()
	bool isSniperZoomed = false;

	UPROPERTY(Replicated)
	int curRifleAmmo = 40;

	UPROPERTY(Replicated)
	int curSniperAmmo = 5;

	UPROPERTY(Replicated)
	int curPistolAmmo = 8;

	UPROPERTY(Replicated)
	int curM249Ammo = 100;

	UPROPERTY()
	bool EmptySoundBoolean = false;

	UPROPERTY(Replicated)
	int maxRifleAmmo;

	UPROPERTY(Replicated)
	int maxSniperAmmo;

	UPROPERTY(Replicated)
	int maxPistolAmmo;

	UPROPERTY(Replicated)
	int maxM249Ammo;

	UPROPERTY()
	float zoomTriggeredTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bStashWidgetOn = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bDeadBodyWidgetOn = false;

	UPROPERTY()
	bool bPlayerDeath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isZooming = false;

	UPROPERTY(EditAnywhere)
	bool IsZoomKeyPressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isRunning = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isRifleZooming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isSniperZooming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isM249Zooming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isCursorOnRifle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isCursorOnSniper;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isCursorOnPistol;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isCursorOnM249;

	UPROPERTY()
	bool IsHideOut = true;

	UPROPERTY()
	bool TickOverlapBoolean = false;

	UPROPERTY()
	bool isRifleShootable;

	UPROPERTY()
	bool OpenMenuBoolean = false;;

	UPROPERTY(Replicated)
	bool CanShoot = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecRifle = 11.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecPistol = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecSniper = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecM249 = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAttackDamageRifle = 9.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAttackDamagePistol = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAttackDamageSniper = 70.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerAttackDamageM249 = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemyAttackDamageRifle = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemyAttackDamagePistol = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemyAttackDamageSniper = 700.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemyAttackDamageM249 = 80.f;

	UPROPERTY(Replicated, VisibleDefaultsOnly)
	float RandPlayerAttackDamageRifle;

	UPROPERTY(Replicated, VisibleDefaultsOnly)
	float RandPlayerAttackDamagePistol;

	UPROPERTY(Replicated, VisibleDefaultsOnly)
	float RandPlayerAttackDamageSniper;

	UPROPERTY(Replicated, VisibleDefaultsOnly)
	float RandPlayerAttackDamageM249;

	UPROPERTY(Replicated, VisibleDefaultsOnly)
	float RandEnemyAttackDamageRifle;

	UPROPERTY(Replicated, VisibleDefaultsOnly)
	float RandEnemyAttackDamagePistol;

	UPROPERTY(Replicated, VisibleDefaultsOnly)
	float RandEnemyAttackDamageSniper;

	UPROPERTY(Replicated, VisibleDefaultsOnly)
	float RandEnemyAttackDamageM249;

	//Sounds
	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* BulletEmptySound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* BulletHitSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* BulletHeadHitSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* KillSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* AmmoPickupSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* PlayerKillSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* RifleFireSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* SniperFireSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* PistolFireSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* M249FireSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* RifleReloadSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* SniperReloadSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* PistolReloadSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* M249ReloadSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* SwapSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* zoomSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* SniperZoomSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* gearEquipSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* gearUnequipSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* DamagedSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* SniperZoomInSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* SniperZoomOutSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* PickUpSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* WeaponSwapSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* tabSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* PortalSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* quitGameSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* levelSelectionSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* ExtractionSound;

	UPROPERTY()
	FTimerHandle shootEnableHandle;

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)
	TSubclassOf<class UCameraShakeBase> sniperCameraShake;

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)
	TSubclassOf<class UCameraShakeBase> sniperZoomingShake;

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)
	TSubclassOf<class UCameraShakeBase> rifleFireShake;

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)
	TSubclassOf<class UCameraShakeBase> pistolFireShake;

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)
	TSubclassOf<class UCameraShakeBase> sniperFireShake;

	UPROPERTY(EditDefaultsOnly, Category=CameraMotion)
	TSubclassOf<class UCameraShakeBase> PlayerDamagedShake;

	//Particles
	UPROPERTY(EditAnywhere, Transient, Category="Factory")
	TSubclassOf<class AActor> ShotDecalFactory;
	UPROPERTY(EditAnywhere, Transient, Category="Particle")
	class UParticleSystem* bulletMarksParticle;
	UPROPERTY(EditAnywhere, Transient, Category="Particle")
	class UParticleSystem* RifleFireParticle;
	UPROPERTY(EditAnywhere, Transient, Category="Particle")
	class UParticleSystem* FirstPersonRifleFireParticle;
	UPROPERTY(EditAnywhere, Transient, Category="Particle")
	class UParticleSystem* PistolfireParticle;
	UPROPERTY(EditAnywhere, Transient, Category="Particle")
	class UParticleSystem* recallParticle;
	UPROPERTY(EditAnywhere, Transient, Category="Particle")
	class UParticleSystem* SniperFireParticle;
	UPROPERTY(EditAnywhere, Transient, Category="Particle")
	class UParticleSystem* BloodParticle;

	UPROPERTY(EditAnywhere) // Timeline 생성
	FTimeline Timeline;

	UPROPERTY(EditAnywhere) // Timeline 커브
	UCurveFloat* CurveFloat;

	UPROPERTY()
	class AEclipseGameMode* gm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UEclipseGameInstance* gi;

	UPROPERTY()
	FTimerHandle SniperZoomHandle;
	UPROPERTY()
	FTimerHandle SniperZoomOutHandle;


	UPROPERTY()
	bool HelmetEquipped = false;

	UPROPERTY()
	bool MaskEquipped = false;

	UPROPERTY()
	bool HeadsetEquipped = false;

	UPROPERTY()
	bool GoggleEquipped = false;

	UPROPERTY()
	bool ArmorEquipped = false;

	UPROPERTY(Replicated)
	bool IsPlayerDeadImmediately = false;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool IsPlayerDead = false;

	UPROPERTY()
	bool bRifleAdditionalMag;
	UPROPERTY()
	bool bSniperAdditionalMag;
	UPROPERTY()
	bool bPistolAdditionalMag;
	UPROPERTY()
	bool bM249AdditionalMag;

	UPROPERTY()
	class AEclipsePlayerController* PC;

	UPROPERTY()
	class AHackingConsole* HackingConsole;

	UPROPERTY()
	class AMissionChecker* MissionChecker;

	UPROPERTY()
	class AStageBoard* StageBoard;

	UPROPERTY()
	class AQuitGameActor* QuitGameActor;

	UPROPERTY()
	class AStash* Stash;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class ARifleMagActor* RifleMagActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class ASniperMagActor* SniperMagActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class APistolMagActor* PistolMagActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class AM249MagActor* M249MagActor;

	UPROPERTY()
	float CharacterWalkSpeed = 360.f;

	UPROPERTY()
	int ConsoleCount;

	UPROPERTY()
	int GuardianCount;

	UPROPERTY()
	int BossCount;

	UPROPERTY()
	bool bGuardian;

	UPROPERTY()
	bool bCrunch;

	UPROPERTY()
	bool bEnding;

	UPROPERTY()
	bool SniperZoomBool;

	UPROPERTY()
	bool SniperZoomOutBool;
};
