// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossHPWidget.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RifleActor.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.generated.h"

DECLARE_DYNAMIC_DELEGATE(FRewardContainerDestruct);

UCLASS()
class ECLIPSE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRewardContainerDestruct containerDele;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for zooming input */
	void Zoom();
	/** Called for zooming input */
	void ZoomRelease();

	/** Called for running input */
	void Run();
	/** Called for running input */
	void RunRelease();
	
	/** Called for fire input */
	void Fire();
	void FireRelease();
	
	/** Called for zooming input */
	void Crouching();

	/** Called for change weapon input */
	void ChangeWeapon();

	void Reload();

	void OnActionLookAroundPressed();
	void OnActionLookAroundReleased();

	void OnZoomIn();
	void OnZoomOut();

	void SwapFirstWeapon();
	void SwapSecondWeapon();

	void Tab();

	void Q();
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* sniperComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* rifleComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* pistolComp;

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
	
	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* RifleBulletShellDropSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class ADamageWidgetActor> damageWidgetFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class UDamageWidget> damageWidgetUIFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<UBossHPWidget> bossHPWidgetFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = widget)
	class UDamageWidget* damageWidgetUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = widget)
	class UBossHPWidget* bossHPUI;

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

	UPROPERTY(EditAnywhere)
	TArray<bool> weaponArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> equippedWeaponStringArray;

	UPROPERTY(BlueprintReadOnly)
	class UCrosshairWidget* crosshairUI;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* bulletImpactFactory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector ContainerLoc;
	
	UPROPERTY(BlueprintReadOnly)
	class UUserWidget* sniperScopeUI;
	
	UPROPERTY()
	class AActor* hitActors;

	UPROPERTY()
	class AEnemy* enemyRef;

	UPROPERTY()
	FVector DeathPosition;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int curWeaponSlotNumber = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TabBool = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool TabOn = false;

	UFUNCTION()
	void WeaponDetectionLineTrace();

	UFUNCTION()
	void SetBossHPWidget(AEnemy* enemy);
	
	UFUNCTION()
	void SetDamageWidget(int damage, FVector spawnLoc);

	UFUNCTION()
	void SetHeadDamageWidget(int damage, FVector spawnLoc);

	UFUNCTION()
	void RemoveBossHPWidget();
	
	UFUNCTION()
	void InfoWidgetUpdate();

	UFUNCTION()
	float DamageMultiplier();

	UFUNCTION()
	float FireRateMultiplier();

	UFUNCTION()
	float RecoilRateMultiplier();

	UFUNCTION()
	void PlayerDeath();

	UFUNCTION(BlueprintCallable)
	void EquipHelmet();

	UFUNCTION(BlueprintCallable)
	void EquipHeadset();

	UFUNCTION(BlueprintCallable)
	void EquipMask();

	UFUNCTION(BlueprintCallable)
	void EquipGoggle();

	UFUNCTION(BlueprintCallable)
	void UnEquipHelmet();

	UFUNCTION(BlueprintCallable)
	void UnEquipHeadset();

	UFUNCTION(BlueprintCallable)
	void UnEquipMask();

	UFUNCTION(BlueprintCallable)
	void UnEquipGoggle();

	UFUNCTION(BlueprintCallable)
	void EquipArmor();

	UFUNCTION(BlueprintCallable)
	void UnEquipArmor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUsingRifle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUsingSniper;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUsingPistol;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUsingM249;
	
	UPROPERTY()
	class UPlayerAnim* animInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	class UAnimMontage* zoomingMontage;
	
	UPROPERTY()
	int curRifleAmmo = 40;

	UPROPERTY()
	int curSniperAmmo = 5;

	UPROPERTY()
	int curPistolAmmo = 8;

	UPROPERTY()
	int curM249Ammo = 100;

	UPROPERTY()
	bool EmptySoundBoolean = false;

	UPROPERTY()
	int maxRifleAmmo = 80;

	UPROPERTY()
	int maxSniperAmmo = 10;

	UPROPERTY()
	int maxPistolAmmo = 16;

	UPROPERTY()
	int maxM249Ammo = 200;

	UPROPERTY()
	float zoomTriggeredTime;

	UPROPERTY()
	bool bPlayerDeath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isZooming = false;

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
	bool TickOverlapBoolean=false;
	
	UPROPERTY()
	bool isRifleShootable;

	UPROPERTY()
	bool CanShoot = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecRifle = 13.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecPistol = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecSniper = 1.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecM249 = 9.0f;

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
	class USoundBase* SwapSound;
	
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

	//Particles
	UPROPERTY(EditAnywhere, Category="Factory")
	TSubclassOf<class AActor> ShotDecalFactory;
	UPROPERTY(EditAnywhere, Category="Factory")
	TSubclassOf<class AActor> BulletShellFactory;
	UPROPERTY(EditAnywhere, Category="Particle")
	class UParticleSystem* bulletMarksParticle;
	UPROPERTY(EditAnywhere, Category="Particle")
	class UParticleSystem* RifleFireParticle;
	UPROPERTY(EditAnywhere, Category="Particle")
	class UParticleSystem* RifleFireParticle2;
	UPROPERTY(EditAnywhere, Category="Particle")
	class UParticleSystem* PistolfireParticle;
	UPROPERTY(EditAnywhere, Category="Particle")
	class UParticleSystem* shieldParticle;
	UPROPERTY(EditAnywhere, Category="Particle")
	class UParticleSystem* recallParticle;
	UPROPERTY(EditAnywhere, Category="Niagara")
	class UNiagaraSystem* BulletTrailSystem;
	UPROPERTY(EditAnywhere, Category="Niagara")
	class UParticleSystem* SniperFireParticle;
	
	
	UPROPERTY(EditAnywhere)  // Timeline 생성
	FTimeline Timeline;					

	UPROPERTY(EditAnywhere)  // Timeline 커브
	UCurveFloat* CurveFloat;  

	UPROPERTY()
	class AEclipseGameMode* gm;
	
	UFUNCTION()  // Bind function
	void SetZoomValue(float Value);

	UFUNCTION()
	void CachingValues();

	UFUNCTION()
	void ApplyCachingValues();

	UFUNCTION()
	void Damaged(int damage);

	UFUNCTION()
	int32 SetRifleAdditionalMagazine();
	UFUNCTION()
	int32 SetSniperAdditionalMagazine();
	UFUNCTION()
	int32 SetPistolAdditionalMagazine();
	UFUNCTION()
	int32 SetM249AdditionalMagazine();

	UPROPERTY()
	bool CoreEquipped = false;

	UPROPERTY()
	int randRifleDamage;
	UPROPERTY()
	int randRifleHeadDamage;
	UPROPERTY()
	int randSniperDamage;
	UPROPERTY()
	int randSniperHeadDamage;
	UPROPERTY()
	int randPistolDamage;
	UPROPERTY()
	int randPistolHeadDamage;
	UPROPERTY()
	int randM249Damage;
	UPROPERTY()
	int randM249HeadDamage;

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

	UPROPERTY()
	bool bRifleAdditionalMag;
	UPROPERTY()
	bool bSniperAdditionalMag;
	UPROPERTY()
	bool bPistolAdditionalMag;
	UPROPERTY()
	bool bM249AdditionalMag;

	UPROPERTY()
	class AEclipsePlayerController* pc;
	
	UPROPERTY()
	class AHackingConsole* HackingConsole;

	UPROPERTY()
	class AMissionChecker* MissionChecker;

	UPROPERTY()
	class ARifleMagActor* RifleMagActor;
	UPROPERTY()
	class ASniperMagActor* SniperMagActor;
	UPROPERTY()
	class APistolMagActor* PistolMagActor;
	UPROPERTY()
	class AM249MagActor* M249MagActor;
	
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
	int curHP;

	UPROPERTY()
	int maxHP = 100.f;
};
