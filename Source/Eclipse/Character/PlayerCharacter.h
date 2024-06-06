// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Eclipse/Weapon/RifleActor.h"
#include "Eclipse/UI/StashWidget.h"
#include "Components/TimelineComponent.h"
#include "Eclipse/Game/EclipseGameInstance.h"
#include "Eclipse/Item/ArmorActor.h"
#include "Eclipse/Prop/DeadPlayerContainer.h"
#include "Eclipse/Prop/RewardContainer.h"
#include "Eclipse/Weapon/M249Actor.h"
#include "Eclipse/Weapon/PistolActor.h"
#include "Eclipse/Weapon/SniperActor.h"
#include "GameFramework/PlayerStart.h"
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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Stat Actor Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPlayerCharacterStatComponent> Stat;

	// Line Trace Detection Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class ULineTraceDetectionComponent> Detection;

	// Flash Light Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Light, Meta = (AllowPrivateAccess = "true"))
	class USpotLightComponent* FlashLight;

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
	
	/** Flash Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FlashAction;

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
	class UInputAction* EAction;

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

	UFUNCTION()
	void FireLocal();

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
	void DeadBodyInteraction(ADeadPlayerContainer* DeadPlayerCharacterBox);
	UFUNCTION(Reliable, Server, WithValidation)
	void DeadBodyInteractionRPCServer(ADeadPlayerContainer* DeadPlayerCharacterBox);
	UFUNCTION(Unreliable, NetMulticast)
	void DeadBodyInteractionRPCMutlicast(ADeadPlayerContainer* DeadPlayerCharacterBox);


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

	UFUNCTION(Unreliable, Client)
	void OnPlayerKillRPCClient();

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

	UFUNCTION(Unreliable, Client)
	void OnEnemyKillRPCClient();

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

	void TiltingLeft();

	UFUNCTION(Reliable, Server, WithValidation)
	void TiltingLeftRPCServer();

	UFUNCTION(Unreliable, NetMulticast)
	void TiltingLeftRPCMulticast();
	
	void TiltingLeftRelease();

	UFUNCTION(Reliable, Server, WithValidation)
	void TiltingLeftReleaseRPCServer();

	UFUNCTION(Unreliable, NetMulticast)
	void TiltingLeftReleaseRPCMulticast();
	
	void TiltingRight();

	UFUNCTION(Reliable, Server, WithValidation)
	void TiltingRightRPCServer();

	UFUNCTION(Unreliable, NetMulticast)
	void TiltingRightRPCMulticast();
	
	void TiltingRightRelease();

	UFUNCTION(Reliable, Server, WithValidation)
	void TiltingRightReleaseRPCServer();

	UFUNCTION(Unreliable, NetMulticast)
	void TiltingRightReleaseRPCMulticast();

	UFUNCTION()
	void SetDamageWidget(int Damage, const FVector& SpawnLoc, bool bIsShieldIconEnable, FLinearColor DamageTextColor);

	UFUNCTION(BlueprintImplementableEvent)
	void StashWidgetOnViewport();

	UFUNCTION(BlueprintImplementableEvent)
	void DeadBodyWidgetOnViewport();

	UFUNCTION()
	void InteractionProcess();

	UFUNCTION()
	void MoveToIsolatedShip();

	UFUNCTION()
	void MoveToIsolatedShipClient();

	UFUNCTION()
	void MoveToHideout(const bool IsPlayerDeath);

	UFUNCTION(Server, Reliable)
	void ResetPlayerInventoryDataServer();

	UFUNCTION()
	void MoveToBlockedIntersection();

	UFUNCTION()
	void MoveToBlockedIntersectionClient();

	UFUNCTION() // Bind function
	void SetZoomValue(float Value);

	UFUNCTION() // Bind function
	void SetTiltingLeftValue(float Value);

	UFUNCTION() // Bind function
	void SetTiltingRightValue(float Value);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateAmmunition();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTabWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTabWidgetHP();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTabWidgetInventory();

	UFUNCTION(BlueprintImplementableEvent)
	void ResetTabWidget();

	UFUNCTION()
	void OnRep_WeaponArrayChanged();

	UFUNCTION()
	void OnRep_IsEquipArmor() const;

	UFUNCTION()
	void OnRep_IsEquipHelmet() const;

	UFUNCTION()
	void OnRep_IsEquipGoggle() const;

	UFUNCTION()
	void OnRep_IsEquipMask() const;

	UFUNCTION()
	void OnRep_IsEquipHeadset() const;

	UFUNCTION()
	void OnRep_MaxRifleAmmo();

	UFUNCTION()
	void OnRep_MaxSniperAmmo();

	UFUNCTION()
	void OnRep_MaxPistolAmmo();

	UFUNCTION()
	void OnRep_MaxM249Ammo();

	UFUNCTION()
	int32 GenerateRandomDamage(float InDamage) const;

	UFUNCTION()
	void AmmoDepleted();

	UFUNCTION()
	void ExtractionSuccess();

	UFUNCTION()
	void SetFirstPersonModeRifle(const bool IsFirstPerson);
	UFUNCTION()
	void SetFirstPersonModePistol(const bool IsFirstPerson);

	UFUNCTION(BlueprintCallable)
	void EquipArmorInventorySlot(bool IsEquipping, const float EquipGearStat);

	UFUNCTION(BlueprintCallable)
	void EquipHelmetInventorySlot(bool IsEquipping, const float EquipGearStat);

	UFUNCTION(BlueprintCallable)
	void EquipGoggleInventorySlot(bool IsEquipping, const float EquipGearStat);

	UFUNCTION(BlueprintCallable)
	void EquipHeadsetInventorySlot(bool IsEquipping, const float EquipGearStat);

	UFUNCTION(BlueprintCallable)
	void EquipMaskInventorySlot(bool IsEquipping, const float EquipGearStat);

	UFUNCTION()
	void DeadPlayerContainerSettings(ADeadPlayerContainer* DeadPlayerContainers) const;

	UFUNCTION()
	void AddAmmunitionByInputString(const FString& InventoryStructName);

	UFUNCTION()
	void OnSpacecraftStreamingLevelLoadFinished();

	UFUNCTION(Server, Reliable)
	void OnSpacecraftStreamingLevelLoadFinishedServer();

	UFUNCTION()
	void OnIntersectionStreamingLevelLoadFinished();

	UFUNCTION(Server, Reliable)
	void OnIntersectionStreamingLevelLoadFinishedServer();

	UFUNCTION()
	void OnHideoutStreamingLevelLoadFinished();

	UFUNCTION(Server, Reliable)
	void OnHideoutStreamingLevelLoadFinishedServer();

	UFUNCTION(NetMulticast, Reliable)
	void OnHideoutStreamingLevelLoadFinishedMulticast();

	UFUNCTION(Client, Unreliable)
	void SetPlayerControlRotation(const FRotator& DesiredRotation);

	UFUNCTION()
	void WidgetConstruction();

	UFUNCTION()
	void MoveToAnotherLevel();

	UFUNCTION(Server, Reliable)
	void MoveToAnotherLevelServer();

	UFUNCTION(NetMulticast, Unreliable)
	void MoveToAnotherLevelMulticast();

	UFUNCTION()
	void ResetFireBoolean();

	UFUNCTION()
	void UnloadMultipleStreamingLevels(const FName& FirstLevelName, const FName& SecondLevelName);

	UFUNCTION(BlueprintCallable)
	void PurchaseAmmo(const int32 AmmoIndex);

	UFUNCTION(Server, Reliable)
	void PurchaseAmmoServer(const int32 AmmoIndex);

	UFUNCTION()
	void ToggleFlashlight();

	UFUNCTION(Server, Reliable)
	void ToggleFlashlightServer();
	
	UFUNCTION(NetMulticast, Unreliable)
	void ToggleFlashlightMulticast();

	UFUNCTION()
	void ModifyFlashlightAttachment(const int32 WeaponNum) const;

	UFUNCTION()
	void InitializeAccumulatedDamage();

	UFUNCTION()
	void ChoosePlayerStartByTagName(const FName& PlayerStartTagName, const int32 DetectionSphereRadius);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* SniperComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* RifleComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* FirstPersonRifleComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* PistolComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* FirstPersonPistolComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* M249Comp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* HeadSetSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* GoggleSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* MaskSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* HelmetSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = weapon)
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
	TSubclassOf<class UExtractionCountdown> ExtractionCountdownWidgetFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class ADeadPlayerContainer> DeadPlayerContainerFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = widget)
	class UTradeWidget* TradeWidgetUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class UTradeWidget> TradeWidgetFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = widget)
	class UMenuWidget* MenuWidgetUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class UMenuWidget> MenuWidgetFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = widget)
	class UExtractionCountdown* ExtractionCountdownUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = widget)
	class UDamageWidget* damageWidgetUI;

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
	class AFirstAidKitActor* FirstAidKitActor;

	UPROPERTY()
	class AAdrenalineSyringe* AdrenalineSyringe;

	UPROPERTY()
	class APoisonOfSpider* PoisonOfSpider;

	UPROPERTY()
	class ARifleActor* rifleActor;

	UPROPERTY()
	class ASniperActor* sniperActor;

	UPROPERTY()
	class APistolActor* pistolActor;

	UPROPERTY()
	class AM249Actor* m249Actor;

	UPROPERTY()
	class AMilitaryLaptop* MilitaryLaptop;

	UPROPERTY()
	class AMilitaryDevice* MilitaryDevice;

	UPROPERTY()
	class ARifleAmmoPack* RifleAmmoPack;

	UPROPERTY()
	class ASniperAmmoPack* SniperAmmoPack;

	UPROPERTY()
	class APistolAmmoPack* PistolAmmoPack;

	UPROPERTY()
	class AM249AmmoPack* M249AmmoPack;

	UPROPERTY()
	class UPlayerAnim* animInstance;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APlayerStart> PlayerStartFactory;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Material)
	class UMaterialInterface* overlayMatRed;

	FVector RifleLineTraceStart;

	FVector RifleLineTraceEnd;

	FVector PistolLineTraceStart;

	FVector PistolLineTraceEnd;

	UPROPERTY()
	FRotator CameraCurrentRotation;

	UPROPERTY()
	FRotator CameraDesiredRotation;

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

	UPROPERTY(ReplicatedUsing=OnRep_MaxRifleAmmo)
	int maxRifleAmmo = 40;

	UPROPERTY(ReplicatedUsing=OnRep_MaxSniperAmmo)
	int maxSniperAmmo = 5;

	UPROPERTY(ReplicatedUsing=OnRep_MaxPistolAmmo)
	int maxPistolAmmo = 8;

	UPROPERTY(ReplicatedUsing=OnRep_MaxM249Ammo)
	int maxM249Ammo = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bStashWidgetOn = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bDeadBodyWidgetOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isZooming = false;

	UPROPERTY(EditAnywhere)
	bool IsZoomKeyPressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isRunning = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isSniperZooming = false;


	UPROPERTY()
	bool TiltReleaseRight = false;

	UPROPERTY()
	bool TiltReleaseLeft = false;

	UPROPERTY(Replicated)
	bool CanShoot = true;

	UPROPERTY(ReplicatedUsing=OnRep_IsEquipArmor)
	bool IsEquipArmor;

	UPROPERTY(ReplicatedUsing=OnRep_IsEquipHelmet)
	bool IsEquipHelmet;

	UPROPERTY(ReplicatedUsing=OnRep_IsEquipHeadset)
	bool IsEquipHeadset;

	UPROPERTY(ReplicatedUsing=OnRep_IsEquipGoggle)
	bool IsEquipGoggle;

	UPROPERTY(ReplicatedUsing=OnRep_IsEquipMask)
	bool IsEquipMask;



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
	class USoundBase* PlayerSpawnSound;

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

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* IntersectionAmbientSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* SpacecraftAmbientSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* FlashlightToggleSound;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* GroundHitSound;
	
	UPROPERTY()
	FTimerHandle ShootEnableHandle;

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

	UPROPERTY()
	UAudioComponent* SpawnedIntersectionSound;
	UPROPERTY()
	UAudioComponent* SpawnedSpacecraftSound;

	UPROPERTY(EditAnywhere) // Timeline 생성
	FTimeline Timeline;

	UPROPERTY(EditAnywhere) // Timeline 커브
	UCurveFloat* CurveFloat;

	UPROPERTY(EditAnywhere) // Timeline 생성
	FTimeline TiltingLeftTimeline;
	
	UPROPERTY(EditAnywhere) // Timeline 생성
	FTimeline TiltingRightTimeline;

	UPROPERTY(EditAnywhere) // Timeline 커브
	UCurveFloat* TiltingCurveFloat;

	UPROPERTY()
	class AEclipseGameMode* gm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UEclipseGameInstance* gi;

	UPROPERTY()
	FTimerHandle SniperZoomHandle;
	UPROPERTY()
	FTimerHandle SniperZoomOutHandle;

	UPROPERTY(Replicated)
	float DamageAmount;

	UPROPERTY()
	bool ArmorEquipped = false;

	UPROPERTY()
	bool bFreeLook = false;

	UPROPERTY(Replicated)
	bool IsPlayerDeadImmediately = false;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool IsPlayerDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bHideout = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bSpacecraft = false;

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

	UPROPERTY()
	class ATrader* Trader;

	UPROPERTY()
	class ADeadPlayerContainer* DeadPlayerContainer;

	UPROPERTY()
	class ARifleMagActor* RifleMagActor;
	
	UPROPERTY()
	class ASniperMagActor* SniperMagActor;
	
	UPROPERTY()
	class APistolMagActor* PistolMagActor;
	
	UPROPERTY()
	class AM249MagActor* M249MagActor;
	
	UPROPERTY()
	float CharacterDefaultWalkSpeed = 360.f;

	UPROPERTY()
	bool bEnding;

	UPROPERTY()
	bool SniperZoomBool;

	UPROPERTY()
	bool SniperZoomOutBool;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float MouseSensitivityRate = 0.5f;

	UPROPERTY()
	bool IsAlreadyDamaged = false;
};
