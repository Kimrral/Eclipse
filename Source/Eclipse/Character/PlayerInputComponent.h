// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnhancedInputComponent.h"
#include "PlayerInputComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECLIPSE_API UPlayerInputComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPlayerInputComponent();

    virtual void SetupInputBindings(UEnhancedInputComponent* EnhancedInputComponent);

protected:
    virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

public:
    /** Called for movement input */
    void Move(const FInputActionValue& Value);

    /** Called for looking input */
    void Look(const FInputActionValue& Value);

    virtual void Jump();
    virtual void StopJumping();
	void Crouching();

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

	UFUNCTION()
	void ToggleFlashlight();

	UFUNCTION(Server, Reliable)
	void ToggleFlashlightServer();
	
	UFUNCTION(NetMulticast, Unreliable)
	void ToggleFlashlightMulticast();	

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
	
	void RemoveAllWidgets();

    //=======================================//

	UFUNCTION()
	void DoorInteraction();

	UFUNCTION(Reliable, Server, WithValidation)
	void DoorInteractionRPCServer();

	UFUNCTION(Unreliable, NetMulticast)
	void DoorInteractionRPCMulticast();

	//=======================================//

	UFUNCTION()
	void ResetFireBoolean();
	
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

    // Input action references
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* JumpAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* ZoomAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* RunAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* CrouchAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* ChangeWeaponAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* FireAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* ReloadAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* LookAroundAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* FirstWeaponSwapAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* SecondWeaponSwapAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* TabAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* MenuAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* QAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* EAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* FlashAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* EscAction;

    UPROPERTY()
    class APlayerCharacter* InputOwner;
};
