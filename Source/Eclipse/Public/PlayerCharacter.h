// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RifleActor.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.generated.h"

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

	/** Crouch Action Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	/** Change Weapon Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponAction;

	/** Reload Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for zooming input */
	void Zoom();
	/** Called for zooming input */
	void ZoomRelease();

	/** Called for fire input */
	void Fire();
	void FireRelease();
	
	/** Called for zooming input */
	void Crouching();

	/** Called for change weapon input */
	void ChangeWeapon();

	void Reload();
	
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
	class UStaticMeshComponent* grenade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* rocketLauncher;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class ARifleActor> rifleFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class ASniperActor> sniperFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class APistolActor> pistolFactory;

	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* RifleBulletShellDropSound;

	UPROPERTY()
	class ARifleActor* rifleActor;

	UPROPERTY()
	class ASniperActor* sniperActor;

	UPROPERTY()
	class APistolActor* pistolActor;

	UPROPERTY(EditAnywhere)		
	TSubclassOf<class UUserWidget> crosshairFactory;

	UPROPERTY(EditAnywhere)
	TArray<bool> weaponArray;

	UPROPERTY(BlueprintReadOnly)
	class UUserWidget* crosshairUI;


	UPROPERTY()
	bool bUsingRifle;
	
	UPROPERTY()
	bool bUsingSniper;

	UPROPERTY()
	bool bUsingPistol;

	UPROPERTY()
	class UPlayerAnim* animInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage)
	class UAnimMontage* zoomingMontage;

	UPROPERTY()
	int curRifleAmmo;

	UPROPERTY()
	int curSniperAmmo;

	UPROPERTY()
	int curPistolAmmo;

	UPROPERTY()
	bool EmptySoundBoolean = false;

	UPROPERTY()
	int maxRifleAmmo;

	UPROPERTY()
	int maxSniperAmmo;

	UPROPERTY()
	int maxPistolAmmo;

	UPROPERTY()
	float zoomTriggeredTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isZooming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isCursorOnRifle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isCursorOnSniper;

	UPROPERTY()
	bool isRifleShootable;

	UPROPERTY()
	bool CanShoot = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecRifle = 11.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletsPerSecPistol = 3.0f;

	//Sounds
	UPROPERTY(EditAnywhere, Category="Sounds")
	class USoundBase* BulletEmptySound;
	
	UPROPERTY()
	FTimerHandle shootEnableHandle;

	//Particles
	UPROPERTY(EditAnywhere, Category="Factory")
	TSubclassOf<class AActor> ShotDecalFactory;
	UPROPERTY(EditAnywhere, Category="Factory")
	TSubclassOf<class AActor> BulletShellFactory;
	UPROPERTY(EditAnywhere, Category="Particle")
	class UParticleSystem* bloodParticle;
	UPROPERTY(EditAnywhere, Category="Particle")
	class UParticleSystem* bulletMarksParticle;
	UPROPERTY(EditAnywhere, Category="Particle")
	class UParticleSystem* fireParticle;
	UPROPERTY(EditAnywhere, Category="Niagara")
	class UNiagaraSystem* BulletTrailSystem;

	UPROPERTY(EditAnywhere)  // Timeline 생성
	FTimeline Timeline;					

	UPROPERTY(EditAnywhere)  // Timeline 커브
	UCurveFloat* CurveFloat;  

	UFUNCTION()  // Bind function
	void SetZoomValue(float Value);
};
