// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "RifleActor.h"
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

	/** Zoom Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ZoomAction;

	/** Crouch Action Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	/** Change Weapon Input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponAction;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for zooming input */
	void Zoom();
	/** Called for zooming input */
	void ZoomRelease();

	/** Called for zooming input */
	void Crouching();

	/** Called for change weapon input */
	void ChangeWeapon();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* sniperComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* rifleComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* grenade;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = weapon)
	class UStaticMeshComponent* rocketLauncher;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class ARifleActor> rifleFactory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = factory)
	TSubclassOf<class ASniperActor> sniperFactory;

	UPROPERTY()
	class ARifleActor* rifleActor;

	UPROPERTY()
	class ASniperActor* sniperActor;

	UPROPERTY()
	TArray<bool> weaponArray;

	UPROPERTY()
	bool bUsingRifle;
	
	UPROPERTY()
	bool bUsingSniper;

	UPROPERTY()
	class UPlayerAnim* animInstance;
	


	UPROPERTY()
	float zoomTriggeredTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isZooming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isCursorOnRifle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool isCursorOnSniper;
};
