// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Character/PlayerInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Image.h"
#include "Components/SpotLightComponent.h"
#include "Eclipse/Animation/FirstPersonPlayerAnim.h"
#include "Eclipse/Animation/PlayerAnim.h"
#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Enemy/Enemy.h"
#include "Eclipse/Item/AdrenalineSyringe.h"
#include "Eclipse/Item/FirstAidKitActor.h"
#include "Eclipse/Item/GoggleActor.h"
#include "Eclipse/Item/HackingConsole.h"
#include "Eclipse/Item/HeadsetActor.h"
#include "Eclipse/Item/HelmetActor.h"
#include "Eclipse/Item/M249AmmoPack.h"
#include "Eclipse/Item/M249MagActor.h"
#include "Eclipse/Item/MaskActor.h"
#include "Eclipse/Item/MedKitActor.h"
#include "Eclipse/Item/MilitaryDevice.h"
#include "Eclipse/Item/MilitaryLaptop.h"
#include "Eclipse/Item/PistolAmmoPack.h"
#include "Eclipse/Item/PistolMagActor.h"
#include "Eclipse/Item/PoisonOfSpider.h"
#include "Eclipse/Item/RifleAmmoPack.h"
#include "Eclipse/Item/RifleMagActor.h"
#include "Eclipse/Item/SniperAmmoPack.h"
#include "Eclipse/Item/SniperMagActor.h"
#include "Eclipse/Player/EclipsePlayerController.h"
#include "Eclipse/UI/CrosshairWidget.h"
#include "Eclipse/UI/InformationWidget.h"
#include "Eclipse/UI/LevelSelection.h"
#include "Eclipse/UI/MenuWidget.h"
#include "Eclipse/UI/QuitWidget.h"
#include "Eclipse/UI/TradeWidget.h"
#include "Eclipse/UI/WeaponInfoWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UPlayerInputComponent::UPlayerInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UPlayerInputComponent::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the owner is a character
	InputOwner = Cast<APlayerCharacter>(GetOwner());
	if (IsValid(InputOwner))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputOwner->InputComponent))
		{
			SetupInputBindings(EnhancedInputComponent);
		}
	}
}

void UPlayerInputComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// 이 컴포넌트가 네트워크에서 복제되도록 설정합니다.
	SetIsReplicated(true);
}

//////////////////////////////////////////////////////////////////////////
// Input
// Called to bind functionality to input
void UPlayerInputComponent::SetupInputBindings(UEnhancedInputComponent* EnhancedInputComponent)
{
	//Jumping
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &UPlayerInputComponent::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &UPlayerInputComponent::StopJumping);

	//Moving
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::Move);

	//Looking
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::Look);

	//Zooming
	EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Started, this, &UPlayerInputComponent::ZoomInput);
	EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Completed, this, &UPlayerInputComponent::ZoomReleaseInput);

	//Running
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &UPlayerInputComponent::Run);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &UPlayerInputComponent::RunRelease);

	//Crouching
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::Crouching);

	//Change Weapon
	EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::ChangeWeapon);
	EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Started, this, &UPlayerInputComponent::DoorInteraction);

	//Fire
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::Fire);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &UPlayerInputComponent::FireRelease);

	//Reload
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &UPlayerInputComponent::Reload);

	//Look Around
	EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::OnActionLookAroundPressed);
	EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Completed, this, &UPlayerInputComponent::OnActionLookAroundReleased);

	//Weapon Swap
	EnhancedInputComponent->BindAction(FirstWeaponSwapAction, ETriggerEvent::Started, this, &UPlayerInputComponent::SwapFirstWeapon);
	EnhancedInputComponent->BindAction(SecondWeaponSwapAction, ETriggerEvent::Started, this, &UPlayerInputComponent::SwapSecondWeapon);

	//Tab
	EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Started, this, &UPlayerInputComponent::Tab);

	//Open Menu
	EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Started, this, &UPlayerInputComponent::OpenMenu);

	//Tilting Left
	EnhancedInputComponent->BindAction(QAction, ETriggerEvent::Started, this, &UPlayerInputComponent::TiltingLeft);
	EnhancedInputComponent->BindAction(QAction, ETriggerEvent::Completed, this, &UPlayerInputComponent::TiltingLeftRelease);

	//Tilting Right
	EnhancedInputComponent->BindAction(EAction, ETriggerEvent::Started, this, &UPlayerInputComponent::TiltingRight);
	EnhancedInputComponent->BindAction(EAction, ETriggerEvent::Completed, this, &UPlayerInputComponent::TiltingRightRelease);

	//Toggle Flashlight
	EnhancedInputComponent->BindAction(FlashAction, ETriggerEvent::Started, this, &UPlayerInputComponent::ToggleFlashlight);

	//Remove Widgets
	EnhancedInputComponent->BindAction(EscAction, ETriggerEvent::Started, this, &UPlayerInputComponent::RemoveAllWidgets);
}


void UPlayerInputComponent::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (InputOwner->Controller != nullptr)
	{
		// find out which way is forward
		const FRotator ControlRotation = InputOwner->Controller->GetControlRotation();
		const FRotator CapsuleRotation = InputOwner->GetCapsuleComponent()->GetComponentRotation();

		const double SelectedYaw = UKismetMathLibrary::SelectFloat(CapsuleRotation.Yaw, ControlRotation.Yaw, InputOwner->bFreeLook);
		const FRotator SelectedRotator(0, SelectedYaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(SelectedRotator).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(SelectedRotator).GetUnitAxis(EAxis::Y);

		// add movement 
		InputOwner->AddMovementInput(ForwardDirection, MovementVector.Y);
		InputOwner->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void UPlayerInputComponent::Look(const FInputActionValue& Value)
{
	if (InputOwner->gi->IsWidgetOn)
	{
		return;
	}
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>() * InputOwner->MouseSensitivityRate;

	if (InputOwner->Controller != nullptr)
	{
		// add yaw and pitch input to controller
		InputOwner->AddControllerYawInput(LookAxisVector.X);
		InputOwner->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void UPlayerInputComponent::Jump()
{
	if (InputOwner->IsPlayerDeadImmediately || InputOwner->bEnding)
	{
		return;
	}
	InputOwner->Super::Jump();
}

void UPlayerInputComponent::StopJumping()
{
	if (InputOwner->IsPlayerDeadImmediately || InputOwner->bEnding)
	{
		return;
	}
	InputOwner->Super::StopJumping();
}

void UPlayerInputComponent::Crouching()
{
}

void UPlayerInputComponent::Tab()
{
}

void UPlayerInputComponent::OpenMenu()
{
	if (InputOwner->MenuWidgetUI)
	{
		if (InputOwner->MenuWidgetUI->IsInViewport() == false)
		{
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(InputOwner->PC, InputOwner->MenuWidgetUI);
			InputOwner->PC->SetShowMouseCursor(true);
			InputOwner->MenuWidgetUI->AddToViewport();
		}
		else
		{
			InputOwner->MenuWidgetUI->CloseWidgetFunc();
		}
	}
}

void UPlayerInputComponent::TiltingLeft()
{
	TiltingLeftRPCServer();
	if (InputOwner->FirstPersonCharacterMesh->IsVisible())
	{
		InputOwner->TiltReleaseLeft = false;
		InputOwner->TiltReleaseRight = false;
		if (InputOwner->TiltingLeftTimeline.IsPlaying())
		{
			InputOwner->TiltingLeftTimeline.Stop();
		}
		if (InputOwner->TiltingRightTimeline.IsPlaying())
		{
			InputOwner->TiltingRightTimeline.Stop();
		}
		InputOwner->TiltingLeftTimeline.PlayFromStart();
	}
}

void UPlayerInputComponent::TiltingLeftRPCServer_Implementation()
{
	TiltingLeftRPCMulticast();
}

bool UPlayerInputComponent::TiltingLeftRPCServer_Validate()
{
	return true;
}

void UPlayerInputComponent::TiltingLeftRPCMulticast_Implementation()
{
	if (!InputOwner->HasAuthority()) InputOwner->animInstance->bTiltingLeft = true;
}

void UPlayerInputComponent::TiltingLeftRelease()
{
	TiltingLeftReleaseRPCServer();
	if (InputOwner->FirstPersonCharacterMesh->IsVisible())
	{
		InputOwner->TiltReleaseLeft = true;
		InputOwner->TiltReleaseRight = true;
		if (InputOwner->TiltingLeftTimeline.IsPlaying())
		{
			InputOwner->TiltingLeftTimeline.Stop();
		}
		if (InputOwner->TiltingRightTimeline.IsPlaying())
		{
			InputOwner->TiltingRightTimeline.Stop();
		}
		InputOwner->TiltingLeftTimeline.PlayFromStart();
	}
}

void UPlayerInputComponent::TiltingLeftReleaseRPCServer_Implementation()
{
	TiltingLeftReleaseRPCMulticast();
}

bool UPlayerInputComponent::TiltingLeftReleaseRPCServer_Validate()
{
	return true;
}

void UPlayerInputComponent::TiltingLeftReleaseRPCMulticast_Implementation()
{
	if (!InputOwner->HasAuthority()) InputOwner->animInstance->bTiltingLeft = false;
}

void UPlayerInputComponent::TiltingRight()
{
	TiltingRightRPCServer();
	if (InputOwner->FirstPersonCharacterMesh->IsVisible())
	{
		InputOwner->TiltReleaseLeft = false;
		InputOwner->TiltReleaseRight = false;
		if (InputOwner->TiltingLeftTimeline.IsPlaying())
		{
			InputOwner->TiltingLeftTimeline.Stop();
		}
		if (InputOwner->TiltingRightTimeline.IsPlaying())
		{
			InputOwner->TiltingRightTimeline.Stop();
		}
		InputOwner->TiltingRightTimeline.PlayFromStart();
	}
}

void UPlayerInputComponent::TiltingRightRPCServer_Implementation()
{
	TiltingRightRPCMulticast();
}

bool UPlayerInputComponent::TiltingRightRPCServer_Validate()
{
	return true;
}

void UPlayerInputComponent::TiltingRightRPCMulticast_Implementation()
{
	if (!InputOwner->HasAuthority()) InputOwner->animInstance->bTiltingRight = true;
}

void UPlayerInputComponent::TiltingRightRelease()
{
	TiltingRightReleaseRPCServer();
	if (InputOwner->FirstPersonCharacterMesh->IsVisible())
	{
		InputOwner->TiltReleaseLeft = true;
		InputOwner->TiltReleaseRight = true;
		if (InputOwner->TiltingLeftTimeline.IsPlaying())
		{
			InputOwner->TiltingLeftTimeline.Stop();
		}
		if (InputOwner->TiltingRightTimeline.IsPlaying())
		{
			InputOwner->TiltingRightTimeline.Stop();
		}
		InputOwner->TiltingRightTimeline.PlayFromStart();
	}
}

void UPlayerInputComponent::TiltingRightReleaseRPCServer_Implementation()
{
	TiltingRightReleaseRPCMulticast();
}

bool UPlayerInputComponent::TiltingRightReleaseRPCServer_Validate()
{
	return true;
}

void UPlayerInputComponent::TiltingRightReleaseRPCMulticast_Implementation()
{
	if (!InputOwner->HasAuthority()) InputOwner->animInstance->bTiltingRight = false;
}

void UPlayerInputComponent::DoorInteraction()
{
	DoorInteractionRPCServer();
}

void UPlayerInputComponent::DoorInteractionRPCServer_Implementation()
{
	DoorInteractionRPCMulticast();
}

bool UPlayerInputComponent::DoorInteractionRPCServer_Validate()
{
	return true;
}

void UPlayerInputComponent::DoorInteractionRPCMulticast_Implementation()
{
	InputOwner->DoorInteractionDele.ExecuteIfBound();
}

void UPlayerInputComponent::Zoom(const bool IsZoomInput)
{
	if (InputOwner->gi->IsWidgetOn || InputOwner->IsPlayerDeadImmediately || InputOwner->bEnding)
	{
		return;
	}
	ZoomRPCServer(IsZoomInput);
}

void UPlayerInputComponent::ZoomRelease(const bool IsZoomInput)
{
	if (InputOwner->gi->IsWidgetOn || InputOwner->IsPlayerDeadImmediately || InputOwner->bEnding)
	{
		return;
	}
	ZoomRPCReleaseServer(IsZoomInput);
}

void UPlayerInputComponent::ZoomInput()
{
	InputOwner->IsZoomKeyPressed = true;
	Zoom(true);
}

void UPlayerInputComponent::ZoomReleaseInput()
{
	InputOwner->IsZoomKeyPressed = false;
	if (InputOwner->TiltingLeftTimeline.IsPlaying())
	{
		InputOwner->TiltingLeftTimeline.Stop();
	}
	if (InputOwner->TiltingRightTimeline.IsPlaying())
	{
		InputOwner->TiltingRightTimeline.Stop();
	}
	InputOwner->FirstPersonCamera->SetRelativeRotation(FRotator::ZeroRotator);
	ZoomRelease(true);
}

void UPlayerInputComponent::ZoomRPCMulticast_Implementation(const bool IsZoomInput)
{
	// Zooming Boolean
	InputOwner->isZooming = true;
	UPlayerAnim* const AnimInst = Cast<UPlayerAnim>(InputOwner->GetMesh()->GetAnimInstance());
	if (AnimInst)
	{
		AnimInst->bZooming = true;
	}
	InputOwner->GetCharacterMovement()->MaxWalkSpeed = InputOwner->CharacterDefaultWalkSpeed;
	InputOwner->GetCharacterMovement()->MaxWalkSpeed = 180.f;

	// is using rifle
	if (InputOwner->weaponArray[0] == true)
	{
		AnimInst->bRifleZooming = true;
		if (InputOwner->IsLocallyControlled())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->zoomSound);
			if (IsZoomInput)
			{
				InputOwner->SetFirstPersonModeRifle(true);
			}
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->zoomSound, InputOwner->GetActorLocation());
		}
	}
	// is using sniper
	else if (InputOwner->weaponArray[1] == true)
	{
		if (InputOwner->IsLocallyControlled())
		{
			InputOwner->isSniperZooming = true;
			InputOwner->Timeline.PlayFromStart();

			InputOwner->crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
			APlayerCameraManager* const cameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
			cameraManager->StartCameraFade(1.0, 0.1, 3.0, FColor::Black, false, true);
			InputOwner->PC->PlayerCameraManager->StartCameraShake(InputOwner->sniperZoomingShake);
			UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->SniperZoomSound);
			// 카메라 줌 러프 타임라인 재생
			InputOwner->sniperScopeUI->AddToViewport();
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->SniperZoomSound, InputOwner->GetActorLocation());
		}
		if (AnimInst)
		{
			AnimInst->bRifleZooming = true;
		}
	}
	else if (InputOwner->weaponArray[2] == true)
	{
		if (InputOwner->IsLocallyControlled())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->zoomSound);
			if (IsZoomInput)
			{
				InputOwner->SetFirstPersonModePistol(true);
			}
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->zoomSound, InputOwner->GetActorLocation());
		}
	}
	else if (InputOwner->weaponArray[3] == true)
	{
		if (InputOwner->IsLocallyControlled())
		{
			if (IsZoomInput)
			{
				InputOwner->Timeline.PlayFromStart();
			}
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->zoomSound, InputOwner->GetActorLocation());
		}
		if (AnimInst)
		{
			AnimInst->bM249Zooming = true;
		}
	}
}

void UPlayerInputComponent::ZoomRPCServer_Implementation(const bool IsZoomInput)
{
	ZoomRPCMulticast(IsZoomInput);
}

bool UPlayerInputComponent::ZoomRPCServer_Validate(const bool IsZoomInput)
{
	return true;
}

void UPlayerInputComponent::ZoomRPCReleaseServer_Implementation(const bool IsZoomInput)
{
	ZoomRPCReleaseMulticast(IsZoomInput);
}

bool UPlayerInputComponent::ZoomRPCReleaseServer_Validate(const bool IsZoomInput)
{
	return true;
}

void UPlayerInputComponent::ZoomRPCReleaseMulticast_Implementation(const bool IsZoomInput)
{
	// Zooming Boolean
	InputOwner->isZooming = false;
	InputOwner->GetCharacterMovement()->MaxWalkSpeed = InputOwner->CharacterDefaultWalkSpeed;
	UPlayerAnim* const AnimInst = Cast<UPlayerAnim>(InputOwner->GetMesh()->GetAnimInstance());
	if (AnimInst)
	{
		AnimInst->bZooming = false;
	}
	if (InputOwner->weaponArray[0] == true)
	{
		AnimInst->bRifleZooming = false;
		if (InputOwner->IsLocallyControlled())
		{
			if (IsZoomInput)
			{
				InputOwner->SetFirstPersonModeRifle(false);
			}
		}
	}
	else if (InputOwner->weaponArray[1] == true)
	{
		if (InputOwner->IsLocallyControlled())
		{
			InputOwner->isSniperZooming = false;
			InputOwner->PC->PlayerCameraManager->StopAllCameraShakes();
			if (InputOwner->GetMesh()->GetAnimInstance()->Montage_IsPlaying(InputOwner->UpperOnlyMontage))
			{
				InputOwner->StopAnimMontage();
			}
			InputOwner->sniperScopeUI->RemoveFromParent();
			InputOwner->crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Visible);
			InputOwner->SniperZoomBool = false;
			InputOwner->SniperZoomOutBool = false;
			InputOwner->Timeline.ReverseFromEnd();
		}
		if (AnimInst)
		{
			AnimInst->bRifleZooming = false;
		}
	}
	else if (InputOwner->weaponArray[2] == true)
	{
		if (InputOwner->IsLocallyControlled())
		{
			if (IsZoomInput)
			{
				InputOwner->SetFirstPersonModePistol(false);
			}
		}
	}
	else if (InputOwner->weaponArray[3] == true)
	{
		if (AnimInst)
		{
			AnimInst->bM249Zooming = false;
		}
		if (InputOwner->IsLocallyControlled())
		{
			if (IsZoomInput)
			{
				InputOwner->Timeline.ReverseFromEnd();
			}
		}
	}
}

void UPlayerInputComponent::Run()
{
	RunRPCServer();
}

void UPlayerInputComponent::RunRelease()
{
	RunRPCReleaseServer();
}

void UPlayerInputComponent::RunRPCMulticast_Implementation()
{
	if (InputOwner->isSniperZooming)
	{
		if (!InputOwner->isSniperZoomed)
		{
			InputOwner->GetWorldTimerManager().ClearTimer(InputOwner->SniperZoomOutHandle);
			InputOwner->SniperZoomBool = true;
			InputOwner->SniperZoomOutBool = false;
			InputOwner->Timeline.Stop();
			InputOwner->FollowCamera->SetFieldOfView(40);
			InputOwner->Timeline.PlayFromStart();
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->SniperZoomInSound, InputOwner->GetActorLocation());
			InputOwner->isSniperZoomed = true;
		}
		else
		{
			InputOwner->GetWorldTimerManager().ClearTimer(InputOwner->SniperZoomHandle);
			InputOwner->SniperZoomOutBool = true;
			InputOwner->SniperZoomBool = false;
			InputOwner->Timeline.Stop();
			InputOwner->FollowCamera->SetFieldOfView(20);
			InputOwner->Timeline.PlayFromStart();
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->SniperZoomOutSound, InputOwner->GetActorLocation());
			InputOwner->isSniperZoomed = false;
		}
		return;
	}
	if (InputOwner->isZooming)
	{
		return;
	}
	InputOwner->GetCharacterMovement()->MaxWalkSpeed = 520.f;
}

void UPlayerInputComponent::RunRPCServer_Implementation()
{
	RunRPCMulticast();
}

bool UPlayerInputComponent::RunRPCServer_Validate()
{
	return true;
}

void UPlayerInputComponent::RunRPCReleaseServer_Implementation()
{
	RunRPCReleaseMulticast();
}

bool UPlayerInputComponent::RunRPCReleaseServer_Validate()
{
	return true;
}

void UPlayerInputComponent::RunRPCReleaseMulticast_Implementation()
{
	if (InputOwner->isZooming)
	{
		return;
	}
	InputOwner->GetCharacterMovement()->MaxWalkSpeed = InputOwner->CharacterDefaultWalkSpeed;
}

void UPlayerInputComponent::OnActionLookAroundPressed()
{
	InputOwner->bFreeLook = true;
	InputOwner->bUseControllerRotationYaw = false;
}

void UPlayerInputComponent::OnActionLookAroundReleased()
{
	if (InputOwner->Controller)
	{
		const FRotator ControlRotation = InputOwner->Controller->GetControlRotation();
		const FRotator CapsuleRotation = InputOwner->GetCapsuleComponent()->GetComponentRotation();
		InputOwner->Controller->SetControlRotation(FRotator(ControlRotation.Pitch, CapsuleRotation.Yaw, ControlRotation.Roll));
	}
	InputOwner->bFreeLook = false;
	InputOwner->bUseControllerRotationYaw = true;
}


void UPlayerInputComponent::ToggleFlashlight()
{
	if (InputOwner->IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->FlashlightToggleSound);
	}

	ToggleFlashlightServer();
}

void UPlayerInputComponent::ToggleFlashlightServer_Implementation()
{
	ToggleFlashlightMulticast();
}

void UPlayerInputComponent::ToggleFlashlightMulticast_Implementation()
{
	if (!InputOwner->HasAuthority())
	{
		if (InputOwner->FlashLight->IsVisible())
		{
			InputOwner->FlashLight->SetVisibility(false);
		}
		else
		{
			InputOwner->FlashLight->SetVisibility(true);
		}
	}
}

void UPlayerInputComponent::ChangeWeapon()
{
	if (InputOwner->bEnding)
	{
		return;
	}
	InputOwner->InteractionProcess();
}

void UPlayerInputComponent::PickableItemActorInteraction(APickableActor* PickableActor)
{
	PickableItemActorInteractionRPCServer(PickableActor);
}

void UPlayerInputComponent::PickableItemActorInteractionRPCServer_Implementation(APickableActor* PickableActor)
{
	PickableItemActorInteractionRPCMutlicast(PickableActor);
}

bool UPlayerInputComponent::PickableItemActorInteractionRPCServer_Validate(APickableActor* PickableActor)
{
	return true;
}

void UPlayerInputComponent::PickableItemActorInteractionRPCMutlicast_Implementation(APickableActor* PickableActor)
{
	if (PickableActor && !PickableActor->IsAlreadyLooted)
	{
		if (!InputOwner->HasAuthority())
		{
			if (InputOwner->IsLocallyControlled())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->PickUpSound);
			}
			else
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->PickUpSound, InputOwner->GetActorLocation());
			}
			InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("WeaponEquip"));
		}
		FTimerHandle DestroyHandle;
		InputOwner->RifleMagActor = Cast<ARifleMagActor>(PickableActor);
		if (InputOwner->RifleMagActor)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->RifleMagActor->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->RifleMagActor->SetLifeSpan(1.f);
				InputOwner->RifleMagActor->SetActorHiddenInGame(true);
				InputOwner->RifleMagActor->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->SniperMagActor = Cast<ASniperMagActor>(PickableActor);
		if (InputOwner->SniperMagActor)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->SniperMagActor->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->SniperMagActor->SetLifeSpan(1.f);
				InputOwner->SniperMagActor->SetActorHiddenInGame(true);
				InputOwner->SniperMagActor->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}
			return;
		}
		InputOwner->PistolMagActor = Cast<APistolMagActor>(PickableActor);
		if (InputOwner->PistolMagActor)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->PistolMagActor->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->PistolMagActor->SetLifeSpan(1.f);
				InputOwner->PistolMagActor->SetActorHiddenInGame(true);
				InputOwner->PistolMagActor->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->M249MagActor = Cast<AM249MagActor>(PickableActor);
		if (InputOwner->M249MagActor)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->M249MagActor->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->M249MagActor->SetLifeSpan(1.f);
				InputOwner->M249MagActor->SetActorHiddenInGame(true);
				InputOwner->M249MagActor->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->GoggleActor = Cast<AGoggleActor>(PickableActor);
		if (InputOwner->GoggleActor)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->GoggleActor->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->GoggleActor->SetLifeSpan(1.f);
				InputOwner->GoggleActor->SetActorHiddenInGame(true);
				InputOwner->GoggleActor->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->HelmetActor = Cast<AHelmetActor>(PickableActor);
		if (InputOwner->HelmetActor)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->HelmetActor->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->HelmetActor->SetLifeSpan(1.f);
				InputOwner->HelmetActor->SetActorHiddenInGame(true);
				InputOwner->HelmetActor->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport())InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->HeadsetActor = Cast<AHeadsetActor>(PickableActor);
		if (InputOwner->HeadsetActor)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->HeadsetActor->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->HeadsetActor->SetLifeSpan(1.f);
				InputOwner->HeadsetActor->SetActorHiddenInGame(true);
				InputOwner->HeadsetActor->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->MaskActor = Cast<AMaskActor>(PickableActor);
		if (InputOwner->MaskActor)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->MaskActor->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->MaskActor->SetLifeSpan(1.f);
				InputOwner->MaskActor->SetActorHiddenInGame(true);
				InputOwner->MaskActor->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->ArmorActor = Cast<AArmorActor>(PickableActor);
		if (InputOwner->ArmorActor)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->ArmorActor->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->ArmorActor->SetLifeSpan(1.f);
				InputOwner->ArmorActor->SetActorHiddenInGame(true);
				InputOwner->ArmorActor->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->MedKitActor = Cast<AMedKitActor>(PickableActor);
		if (InputOwner->MedKitActor)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->MedKitActor->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->MedKitActor->SetLifeSpan(1.f);
				InputOwner->MedKitActor->SetActorHiddenInGame(true);
				InputOwner->MedKitActor->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->FirstAidKitActor = Cast<AFirstAidKitActor>(PickableActor);
		if (InputOwner->FirstAidKitActor)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->FirstAidKitActor->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->FirstAidKitActor->SetLifeSpan(1.f);
				InputOwner->FirstAidKitActor->SetActorHiddenInGame(true);
				InputOwner->FirstAidKitActor->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->AdrenalineSyringe = Cast<AAdrenalineSyringe>(PickableActor);
		if (InputOwner->AdrenalineSyringe)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->AdrenalineSyringe->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->AdrenalineSyringe->SetLifeSpan(1.f);
				InputOwner->AdrenalineSyringe->SetActorHiddenInGame(true);
				InputOwner->AdrenalineSyringe->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->PoisonOfSpider = Cast<APoisonOfSpider>(PickableActor);
		if (InputOwner->PoisonOfSpider)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->PoisonOfSpider->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->PoisonOfSpider->SetLifeSpan(1.f);
				InputOwner->PoisonOfSpider->SetActorHiddenInGame(true);
				InputOwner->PoisonOfSpider->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->MilitaryLaptop = Cast<AMilitaryLaptop>(PickableActor);
		if (InputOwner->MilitaryLaptop)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->MilitaryLaptop->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->MilitaryLaptop->SetLifeSpan(1.f);
				InputOwner->MilitaryLaptop->SetActorHiddenInGame(true);
				InputOwner->MilitaryLaptop->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->MilitaryDevice = Cast<AMilitaryDevice>(PickableActor);
		if (InputOwner->MilitaryDevice)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->MilitaryDevice->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->MilitaryDevice->SetLifeSpan(1.f);
				InputOwner->MilitaryDevice->SetActorHiddenInGame(true);
				InputOwner->MilitaryDevice->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->RifleAmmoPack = Cast<ARifleAmmoPack>(PickableActor);
		if (InputOwner->RifleAmmoPack)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->RifleAmmoPack->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->RifleAmmoPack->SetLifeSpan(1.f);
				InputOwner->RifleAmmoPack->SetActorHiddenInGame(true);
				InputOwner->maxRifleAmmo += 40;
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->SniperAmmoPack = Cast<ASniperAmmoPack>(PickableActor);
		if (InputOwner->SniperAmmoPack)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->SniperAmmoPack->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->SniperAmmoPack->SetLifeSpan(1.f);
				InputOwner->SniperAmmoPack->SetActorHiddenInGame(true);
				InputOwner->maxSniperAmmo += 5;
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport())InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->PistolAmmoPack = Cast<APistolAmmoPack>(PickableActor);
		if (InputOwner->PistolAmmoPack)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->PistolAmmoPack->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->PistolAmmoPack->SetLifeSpan(1.f);
				InputOwner->PistolAmmoPack->SetActorHiddenInGame(true);
				InputOwner->maxPistolAmmo += 8;
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->M249AmmoPack = Cast<AM249AmmoPack>(PickableActor);
		if (InputOwner->M249AmmoPack)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->M249AmmoPack->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->M249AmmoPack->SetLifeSpan(1.f);
				InputOwner->M249AmmoPack->SetActorHiddenInGame(true);
				InputOwner->maxM249Ammo += 100;
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		InputOwner->HackingConsole = Cast<AHackingConsole>(PickableActor);
		if (InputOwner->HackingConsole)
		{
			if (InputOwner->HasAuthority())
			{
				InputOwner->HackingConsole->IsAlreadyLooted = true;
				InputOwner->SetActorTickEnabled(false);
				InputOwner->HackingConsole->SetLifeSpan(1.f);
				InputOwner->HackingConsole->SetActorHiddenInGame(true);
				InputOwner->HackingConsole->AddToInventory(InputOwner);
				InputOwner->GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					InputOwner->SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (InputOwner->IsLocallyControlled())
			{
				if (InputOwner->infoWidgetUI->IsInViewport()) InputOwner->infoWidgetUI->RemoveFromParent();
			}
		}
	}
}


void UPlayerInputComponent::SwapFirstWeapon()
{
	if (InputOwner->curWeaponSlotNumber == 1 || InputOwner->isSniperZooming || InputOwner->animInstance->IsAnyMontagePlaying() || InputOwner->FirstPersonCharacterMesh->IsVisible())
	{
		return;
	}

	SwapFirstWeaponRPCServer();
}

void UPlayerInputComponent::SwapFirstWeaponRPCServer_Implementation()
{
	SwapFirstWeaponRPCMulticast();
}

bool UPlayerInputComponent::SwapFirstWeaponRPCServer_Validate()
{
	return true;
}

void UPlayerInputComponent::SwapFirstWeaponRPCMulticast_Implementation()
{
	if (InputOwner->IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->SwapSound);
		UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->WeaponSwapSound);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->SwapSound, InputOwner->GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->WeaponSwapSound, InputOwner->GetActorLocation());
	}
	InputOwner->curWeaponSlotNumber = 1;

	if (InputOwner->equippedWeaponStringArray[0] == FString("Rifle"))
	{
		InputOwner->animInstance->bPistol = false;
		InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("WeaponEquip"));
		// Visibility 설정
		InputOwner->RifleComp->SetVisibility(true);
		InputOwner->SniperComp->SetVisibility(false);
		InputOwner->PistolComp->SetVisibility(false);
		InputOwner->M249Comp->SetVisibility(false);
		// 무기 배열 설정
		InputOwner->weaponArray[0] = true;
		InputOwner->weaponArray[1] = false;
		InputOwner->weaponArray[2] = false;
		InputOwner->weaponArray[3] = false;

		if (InputOwner->IsLocallyControlled())
		{
			InputOwner->informationUI->PlayAnimation(InputOwner->informationUI->WeaponSwap);
		}
		InputOwner->informationUI->UpdateAmmo_Secondary();
	}
	else if (InputOwner->equippedWeaponStringArray[0] == FString("Sniper"))
	{
		InputOwner->animInstance->bPistol = false;
		InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("WeaponEquip"));
		// Visibility 설정
		InputOwner->RifleComp->SetVisibility(false);
		InputOwner->SniperComp->SetVisibility(true);
		InputOwner->PistolComp->SetVisibility(false);
		InputOwner->M249Comp->SetVisibility(false);
		// 무기 배열 설정
		InputOwner->weaponArray[0] = false;
		InputOwner->weaponArray[1] = true;
		InputOwner->weaponArray[2] = false;
		InputOwner->weaponArray[3] = false;

		if (InputOwner->IsLocallyControlled())
		{
			InputOwner->informationUI->PlayAnimation(InputOwner->informationUI->WeaponSwap);
		}
		InputOwner->informationUI->UpdateAmmo_Secondary();
	}
	else if (InputOwner->equippedWeaponStringArray[0] == FString("Pistol"))
	{
		InputOwner->animInstance->bPistol = true;
		InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("PistolEquip"));
		// Visibility 설정
		InputOwner->RifleComp->SetVisibility(false);
		InputOwner->SniperComp->SetVisibility(false);
		InputOwner->PistolComp->SetVisibility(true);
		InputOwner->M249Comp->SetVisibility(false);
		// 무기 배열 설정
		InputOwner->weaponArray[0] = false;
		InputOwner->weaponArray[1] = false;
		InputOwner->weaponArray[2] = true;
		InputOwner->weaponArray[3] = false;

		if (InputOwner->IsLocallyControlled())
		{
			InputOwner->informationUI->PlayAnimation(InputOwner->informationUI->WeaponSwap);
		}
		InputOwner->informationUI->UpdateAmmo_Secondary();
	}
	else if (InputOwner->equippedWeaponStringArray[0] == FString("M249"))
	{
		InputOwner->animInstance->bPistol = false;
		InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("WeaponEquip"));
		// Visibility 설정
		InputOwner->RifleComp->SetVisibility(false);
		InputOwner->SniperComp->SetVisibility(false);
		InputOwner->PistolComp->SetVisibility(false);
		InputOwner->M249Comp->SetVisibility(true);
		// 무기 배열 설정
		InputOwner->weaponArray[0] = false;
		InputOwner->weaponArray[1] = false;
		InputOwner->weaponArray[2] = false;
		InputOwner->weaponArray[3] = true;

		if (InputOwner->IsLocallyControlled())
		{
			InputOwner->informationUI->PlayAnimation(InputOwner->informationUI->WeaponSwap);
		}
		InputOwner->informationUI->UpdateAmmo_Secondary();
	}
}

void UPlayerInputComponent::SwapSecondWeapon()
{
	if (InputOwner->curWeaponSlotNumber == 2 || InputOwner->isSniperZooming || InputOwner->animInstance->IsAnyMontagePlaying() || InputOwner->FirstPersonCharacterMesh->IsVisible())
	{
		return;
	}
	SwapSecondWeaponRPCServer();
}

void UPlayerInputComponent::SwapSecondWeaponRPCServer_Implementation()
{
	SwapSecondWeaponRPCMulticast();
}

bool UPlayerInputComponent::SwapSecondWeaponRPCServer_Validate()
{
	return true;
}

void UPlayerInputComponent::SwapSecondWeaponRPCMulticast_Implementation()
{
	if (InputOwner->IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->SwapSound);
		UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->WeaponSwapSound);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->SwapSound, InputOwner->GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->WeaponSwapSound, InputOwner->GetActorLocation());
	}

	InputOwner->curWeaponSlotNumber = 2;

	if (InputOwner->equippedWeaponStringArray[1] == FString("Rifle"))
	{
		InputOwner->animInstance->bPistol = false;
		InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("WeaponEquip"));
		// Visibility 설정
		InputOwner->RifleComp->SetVisibility(true);
		InputOwner->SniperComp->SetVisibility(false);
		InputOwner->PistolComp->SetVisibility(false);
		InputOwner->M249Comp->SetVisibility(false);
		// 무기 배열 설정
		InputOwner->weaponArray[0] = true;
		InputOwner->weaponArray[1] = false;
		InputOwner->weaponArray[2] = false;
		InputOwner->weaponArray[3] = false;

		if (InputOwner->IsLocallyControlled())
		{
			InputOwner->informationUI->PlayAnimation(InputOwner->informationUI->WeaponSwap);
		}
		InputOwner->informationUI->UpdateAmmo_Secondary();
	}
	else if (InputOwner->equippedWeaponStringArray[1] == FString("Sniper"))
	{
		InputOwner->animInstance->bPistol = false;
		InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("WeaponEquip"));
		// Visibility 설정
		InputOwner->RifleComp->SetVisibility(false);
		InputOwner->SniperComp->SetVisibility(true);
		InputOwner->PistolComp->SetVisibility(false);
		InputOwner->M249Comp->SetVisibility(false);
		// 무기 배열 설정
		InputOwner->weaponArray[0] = false;
		InputOwner->weaponArray[1] = true;
		InputOwner->weaponArray[2] = false;
		InputOwner->weaponArray[3] = false;

		if (InputOwner->IsLocallyControlled())
		{
			InputOwner->informationUI->PlayAnimation(InputOwner->informationUI->WeaponSwap);
		}
		InputOwner->informationUI->UpdateAmmo_Secondary();
	}
	else if (InputOwner->equippedWeaponStringArray[1] == FString("Pistol"))
	{
		InputOwner->animInstance->bPistol = true;
		InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("PistolEquip"));
		// Visibility 설정
		InputOwner->RifleComp->SetVisibility(false);
		InputOwner->SniperComp->SetVisibility(false);
		InputOwner->PistolComp->SetVisibility(true);
		InputOwner->M249Comp->SetVisibility(false);
		// 무기 배열 설정
		InputOwner->weaponArray[0] = false;
		InputOwner->weaponArray[1] = false;
		InputOwner->weaponArray[2] = true;
		InputOwner->weaponArray[3] = false;

		if (InputOwner->IsLocallyControlled())
		{
			InputOwner->informationUI->PlayAnimation(InputOwner->informationUI->WeaponSwap);
		}
		InputOwner->informationUI->UpdateAmmo_Secondary();
	}
	else if (InputOwner->equippedWeaponStringArray[1] == FString("M249"))
	{
		InputOwner->animInstance->bPistol = false;
		InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("WeaponEquip"));
		// Visibility 설정
		InputOwner->RifleComp->SetVisibility(false);
		InputOwner->SniperComp->SetVisibility(false);
		InputOwner->PistolComp->SetVisibility(false);
		InputOwner->M249Comp->SetVisibility(true);
		// 무기 배열 설정
		InputOwner->weaponArray[0] = false;
		InputOwner->weaponArray[1] = false;
		InputOwner->weaponArray[2] = false;
		InputOwner->weaponArray[3] = true;

		if (InputOwner->IsLocallyControlled())
		{
			InputOwner->informationUI->PlayAnimation(InputOwner->informationUI->WeaponSwap);
		}
		InputOwner->informationUI->UpdateAmmo_Secondary();
	}
}

void UPlayerInputComponent::ChangeWeaponToRifle(ARifleActor* RifleActor)
{
	ChangeWeaponToRifleRPCServer(RifleActor);
}

void UPlayerInputComponent::ChangeWeaponToRifleRPCServer_Implementation(ARifleActor* RifleActor)
{
	ChangeWeaponToRifleRPCMulticast(RifleActor);
}

bool UPlayerInputComponent::ChangeWeaponToRifleRPCServer_Validate(ARifleActor* RifleActor)
{
	return true;
}

void UPlayerInputComponent::ChangeWeaponToRifleRPCMulticast_Implementation(ARifleActor* RifleActor)
{
	if (InputOwner->HasAuthority())
	{
		// 교체 대상 무기 액터 파괴
		RifleActor->Destroy();
	}
	// 무기 교체 Montage 재생
	InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("WeaponEquip"));
	// 액터 스폰 지점 할당
	const FVector spawnPosition = InputOwner->GetMesh()->GetSocketLocation(FName("hand_r"));
	const FRotator spawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// 스나이퍼를 사용중일 때
	if (InputOwner->weaponArray[1] == true)
	{
		// 사용중인 무기 액터 스폰
		GetWorld()->SpawnActor<ASniperActor>(InputOwner->sniperFactory, spawnPosition, spawnRotation, param);
	}
	// 권총을 사용중일 때
	else if (InputOwner->weaponArray[2] == true)
	{
		InputOwner->animInstance->bPistol = false;
		// 사용중인 무기 액터 스폰
		GetWorld()->SpawnActor<APistolActor>(InputOwner->pistolFactory, spawnPosition, spawnRotation, param);
	}
	else if (InputOwner->weaponArray[3] == true)
	{
		// 사용중인 무기 액터 스폰
		GetWorld()->SpawnActor<AM249Actor>(InputOwner->M249Factory, spawnPosition, spawnRotation, param);
	}
	// 현재 활성화된 슬롯이 1번이라면
	if (InputOwner->curWeaponSlotNumber == 1)
	{
		// 장착 무기 이름 배열에 할당
		InputOwner->equippedWeaponStringArray[0] = FString("Rifle");
	}
	// 현재 활성화된 슬롯이 2번이라면
	else if (InputOwner->curWeaponSlotNumber == 2)
	{
		// 장착 무기 이름 배열에 할당
		InputOwner->equippedWeaponStringArray[1] = FString("Rifle");
	}
	// Visibility 설정
	InputOwner->RifleComp->SetVisibility(true);
	InputOwner->SniperComp->SetVisibility(false);
	InputOwner->PistolComp->SetVisibility(false);
	InputOwner->M249Comp->SetVisibility(false);
	// 무기 배열 설정
	InputOwner->weaponArray[0] = true;
	InputOwner->weaponArray[1] = false;
	InputOwner->weaponArray[2] = false;
	InputOwner->weaponArray[3] = false;

	if (!InputOwner->HasAuthority())
	{
		if (InputOwner->IsLocallyControlled())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->WeaponSwapSound);
			// 무기 정보 위젯 제거
			InputOwner->infoWidgetUI->RemoveFromParent();
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->WeaponSwapSound, InputOwner->GetActorLocation());
		}
	}
}

void UPlayerInputComponent::ChangeWeaponToSniper(ASniperActor* SniperActor)
{
	ChangeWeaponToSniperRPCServer(SniperActor);
}

void UPlayerInputComponent::ChangeWeaponToSniperRPCServer_Implementation(ASniperActor* SniperActor)
{
	ChangeWeaponToSniperRPCMulticast(SniperActor);
}

bool UPlayerInputComponent::ChangeWeaponToSniperRPCServer_Validate(ASniperActor* SniperActor)
{
	return true;
}

void UPlayerInputComponent::ChangeWeaponToSniperRPCMulticast_Implementation(ASniperActor* SniperActor)
{
	if (InputOwner->HasAuthority())
	{
		SniperActor->Destroy();
	}
	InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("WeaponEquip"));
	const FVector spawnPosition = InputOwner->GetMesh()->GetSocketLocation(FName("hand_r"));
	const FRotator spawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (InputOwner->weaponArray[0] == true)
	{
		GetWorld()->SpawnActor<ARifleActor>(InputOwner->rifleFactory, spawnPosition, spawnRotation, param);
	}
	else if (InputOwner->weaponArray[2] == true)
	{
		InputOwner->animInstance->bPistol = false;
		GetWorld()->SpawnActor<APistolActor>(InputOwner->pistolFactory, spawnPosition, spawnRotation, param);
	}
	else if (InputOwner->weaponArray[3] == true)
	{
		GetWorld()->SpawnActor<AM249Actor>(InputOwner->M249Factory, spawnPosition, spawnRotation, param);
	}
	if (InputOwner->curWeaponSlotNumber == 1)
	{
		InputOwner->equippedWeaponStringArray[0] = FString("Sniper");
	}
	else if (InputOwner->curWeaponSlotNumber == 2)
	{
		InputOwner->equippedWeaponStringArray[1] = FString("Sniper");
	}
	InputOwner->RifleComp->SetVisibility(false);
	InputOwner->SniperComp->SetVisibility(true);
	InputOwner->PistolComp->SetVisibility(false);
	InputOwner->M249Comp->SetVisibility(false);

	InputOwner->weaponArray[0] = false;
	InputOwner->weaponArray[1] = true;
	InputOwner->weaponArray[2] = false;
	InputOwner->weaponArray[3] = false;

	if (!InputOwner->HasAuthority())
	{
		if (InputOwner->IsLocallyControlled())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->WeaponSwapSound);
			// 무기 정보 위젯 제거
			InputOwner->infoWidgetUI->RemoveFromParent();
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->WeaponSwapSound, InputOwner->GetActorLocation());
		}
	}
}

void UPlayerInputComponent::ChangeWeaponToPistol(APistolActor* PistolActor)
{
	ChangeWeaponToPistolRPCServer(PistolActor);
}

void UPlayerInputComponent::ChangeWeaponToPistolRPCServer_Implementation(APistolActor* PistolActor)
{
	ChangeWeaponToPistolRPCMulticast(PistolActor);
}

bool UPlayerInputComponent::ChangeWeaponToPistolRPCServer_Validate(APistolActor* PistolActor)
{
	return true;
}

void UPlayerInputComponent::ChangeWeaponToPistolRPCMulticast_Implementation(APistolActor* PistolActor)
{
	if (InputOwner->HasAuthority())
	{
		PistolActor->Destroy();
	}
	InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("PistolEquip"));
	InputOwner->animInstance->bPistol = true;
	const FVector spawnPosition = InputOwner->GetMesh()->GetSocketLocation(FName("hand_r"));
	const FRotator spawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (InputOwner->weaponArray[0] == true)
	{
		GetWorld()->SpawnActor<ARifleActor>(InputOwner->rifleFactory, spawnPosition, spawnRotation, param);
	}
	else if (InputOwner->weaponArray[1] == true)
	{
		GetWorld()->SpawnActor<ASniperActor>(InputOwner->sniperFactory, spawnPosition, spawnRotation, param);
	}
	else if (InputOwner->weaponArray[3] == true)
	{
		GetWorld()->SpawnActor<AM249Actor>(InputOwner->M249Factory, spawnPosition, spawnRotation, param);
	}
	if (InputOwner->curWeaponSlotNumber == 1)
	{
		InputOwner->equippedWeaponStringArray[0] = FString("Pistol");
	}
	else if (InputOwner->curWeaponSlotNumber == 2)
	{
		InputOwner->equippedWeaponStringArray[1] = FString("Pistol");
	}
	InputOwner->RifleComp->SetVisibility(false);
	InputOwner->SniperComp->SetVisibility(false);
	InputOwner->PistolComp->SetVisibility(true);
	InputOwner->M249Comp->SetVisibility(false);

	InputOwner->weaponArray[0] = false;
	InputOwner->weaponArray[1] = false;
	InputOwner->weaponArray[2] = true;
	InputOwner->weaponArray[3] = false;

	if (!InputOwner->HasAuthority())
	{
		if (InputOwner->IsLocallyControlled())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->WeaponSwapSound);
			// 무기 정보 위젯 제거
			InputOwner->infoWidgetUI->RemoveFromParent();
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->WeaponSwapSound, InputOwner->GetActorLocation());
		}
	}
}

void UPlayerInputComponent::ChangeWeaponToM249(AM249Actor* M249Actor)
{
	ChangeWeaponToM249RPCServer(M249Actor);
}

void UPlayerInputComponent::ChangeWeaponToM249RPCServer_Implementation(AM249Actor* M249Actor)
{
	ChangeWeaponToM249RPCMulticast(M249Actor);
}

bool UPlayerInputComponent::ChangeWeaponToM249RPCServer_Validate(AM249Actor* M249Actor)
{
	return true;
}

void UPlayerInputComponent::ChangeWeaponToM249RPCMulticast_Implementation(AM249Actor* M249Actor)
{
	if (InputOwner->HasAuthority())
	{
		M249Actor->Destroy();
	}
	InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("WeaponEquip"));
	const FVector spawnPosition = InputOwner->GetMesh()->GetSocketLocation(FName("hand_r"));
	const FRotator spawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (InputOwner->weaponArray[0] == true)
	{
		GetWorld()->SpawnActor<ARifleActor>(InputOwner->rifleFactory, spawnPosition, spawnRotation);
	}
	else if (InputOwner->weaponArray[1] == true)
	{
		GetWorld()->SpawnActor<ASniperActor>(InputOwner->sniperFactory, spawnPosition, spawnRotation);
	}
	else if (InputOwner->weaponArray[2] == true)
	{
		InputOwner->animInstance->bPistol = false;
		GetWorld()->SpawnActor<APistolActor>(InputOwner->pistolFactory, spawnPosition, spawnRotation);
	}
	if (InputOwner->curWeaponSlotNumber == 1)
	{
		InputOwner->equippedWeaponStringArray[0] = FString("M249");
	}
	else if (InputOwner->curWeaponSlotNumber == 2)
	{
		InputOwner->equippedWeaponStringArray[1] = FString("M249");
	}
	InputOwner->RifleComp->SetVisibility(false);
	InputOwner->SniperComp->SetVisibility(false);
	InputOwner->PistolComp->SetVisibility(false);
	InputOwner->M249Comp->SetVisibility(true);

	InputOwner->weaponArray[0] = false;
	InputOwner->weaponArray[1] = false;
	InputOwner->weaponArray[2] = false;
	InputOwner->weaponArray[3] = true;

	if (!InputOwner->HasAuthority())
	{
		if (InputOwner->IsLocallyControlled())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->WeaponSwapSound);
			// 무기 정보 위젯 제거
			InputOwner->infoWidgetUI->RemoveFromParent();
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->WeaponSwapSound, InputOwner->GetActorLocation());
		}
	}
}


void UPlayerInputComponent::Reload()
{
	ServerRPCReload();
}

void UPlayerInputComponent::ServerRPCReload_Implementation()
{
	MulticastRPCReload();
}

void UPlayerInputComponent::MulticastRPCReload_Implementation()
{
	if (const bool IsMontagePlaying = InputOwner->animInstance->IsAnyMontagePlaying(); !IsMontagePlaying)
	{
		if (InputOwner->weaponArray[0] == true && InputOwner->curRifleAmmo < 40 && InputOwner->maxRifleAmmo > 0)
		{
			InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("Reload"));

			if (InputOwner->IsLocallyControlled())
			{
				InputOwner->SetFirstPersonModeRifle(false);
				InputOwner->crosshairUI->PlayAnimation(InputOwner->crosshairUI->ReloadAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
				UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->RifleReloadSound);
			}
			else
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->RifleReloadSound, InputOwner->GetActorLocation());
			}
		}
		else if (InputOwner->weaponArray[1] == true && InputOwner->curSniperAmmo < 5 && InputOwner->maxSniperAmmo > 0)
		{
			InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("Reload"));

			if (InputOwner->IsLocallyControlled())
			{
				InputOwner->crosshairUI->PlayAnimation(InputOwner->crosshairUI->ReloadAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
				UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->SniperReloadSound);
			}
			else
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->SniperReloadSound, InputOwner->GetActorLocation());
			}
		}
		else if (InputOwner->weaponArray[2] == true && InputOwner->curPistolAmmo < 8 && InputOwner->maxPistolAmmo > 0)
		{
			InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("PistolReload"));

			if (InputOwner->IsLocallyControlled())
			{
				InputOwner->SetFirstPersonModePistol(false);
				InputOwner->crosshairUI->PlayAnimation(InputOwner->crosshairUI->ReloadAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
				UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->PistolReloadSound);
			}
			else
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->PistolReloadSound, InputOwner->GetActorLocation());
			}
		}
		else if (InputOwner->weaponArray[3] == true && InputOwner->curM249Ammo < 100 && InputOwner->maxM249Ammo > 0)
		{
			InputOwner->PlayAnimMontage(InputOwner->UpperOnlyMontage, 1, FName("M249Reload"));

			if (InputOwner->IsLocallyControlled())
			{
				InputOwner->crosshairUI->PlayAnimation(InputOwner->crosshairUI->ReloadAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
				UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->M249ReloadSound);
			}
			else
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->M249ReloadSound, InputOwner->GetActorLocation());
			}
		}
	}
}


void UPlayerInputComponent::ResetFireBoolean()
{
	InputOwner->CanShoot = true;
}

void UPlayerInputComponent::Fire()
{
	if (!InputOwner->CanShoot || InputOwner->isRunning || InputOwner->gi->IsWidgetOn || InputOwner->bEnding || InputOwner->IsPlayerDeadImmediately)
	{
		return;
	}
	if (InputOwner->bHideout)
	{
		InputOwner->crosshairUI->PlayAnimation(InputOwner->crosshairUI->HideoutWarningAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
		return;
	}
	if (!InputOwner->isZooming && InputOwner->weaponArray[1] == false && InputOwner->weaponArray[2] == false)
	{
		InputOwner->GetWorldTimerManager().ClearTimer(InputOwner->ZoomFireHandle);
		Zoom(false);
	}
	InputOwner->CanShoot = false;
	FireLocal();
	ServerRPCFire();
	InputOwner->GetWorldTimerManager().SetTimer(InputOwner->ShootEnableHandle, this, &UPlayerInputComponent::ResetFireBoolean, InputOwner->Stat->GetFireInterval(InputOwner->weaponArray), false);
}

void UPlayerInputComponent::FireLocal()
{
	// Rifle
	if (InputOwner->weaponArray[0] == true)
	{
		if (InputOwner->curRifleAmmo > 0)
		{
			ProcessRifleFireLocal();
		}
	}
	// Sniper
	else if (InputOwner->weaponArray[1] == true)
	{
		if (InputOwner->curSniperAmmo > 0)
		{
			ProcessSniperFireLocal();
		}
	}
	// Pistol
	else if (InputOwner->weaponArray[2] == true)
	{
		if (InputOwner->curPistolAmmo > 0)
		{
			ProcessPistolFireLocal();
		}
	}
	// M249
	else if (InputOwner->weaponArray[3] == true)
	{
		if (InputOwner->curM249Ammo > 0)
		{
			ProcessM249FireLocal();
		}
	}
}

bool UPlayerInputComponent::ServerRPCFire_Validate()
{
	return true;
}

void UPlayerInputComponent::ServerRPCFire_Implementation()
{
	MulticastRPCFire();
}

void UPlayerInputComponent::MulticastRPCFire_Implementation()
{
	// Rifle
	if (InputOwner->weaponArray[0] == true)
	{
		if (InputOwner->curRifleAmmo > 0)
		{
			ProcessRifleFireAnim();

			if (!InputOwner->IsLocallyControlled())
			{
				ProcessRifleFireSimulatedProxy();
			}

			// 서버 로직 (핵심 프로세스 처리)
			if (InputOwner->HasAuthority())
			{
				ProcessRifleFire();
			}
		}
		// No Ammo
		else
		{
			InputOwner->AmmoDepleted();
		}
		return;
	}
	// Sniper
	if (InputOwner->weaponArray[1] == true)
	{
		if (InputOwner->curSniperAmmo > 0)
		{
			ProcessSniperFireAnim();

			if (!InputOwner->IsLocallyControlled())
			{
				ProcessSniperFireSimulatedProxy();
			}

			// 서버 로직 (핵심 프로세스 처리)
			if (InputOwner->HasAuthority())
			{
				ProcessSniperFire();
			}
		}
		// No Ammo
		else
		{
			InputOwner->AmmoDepleted();
		}
		return;
	}
	// Pistol
	if (InputOwner->weaponArray[2] == true)
	{
		if (InputOwner->curPistolAmmo > 0)
		{
			ProcessPistolFireAnim();

			if (!InputOwner->IsLocallyControlled())
			{
				ProcessPistolFireSimulatedProxy();
			}

			// 서버 로직 (핵심 프로세스 처리)
			if (InputOwner->HasAuthority())
			{
				ProcessPistolFire();
			}
		}
		else
		{
			InputOwner->AmmoDepleted();
		}
		return;
	}
	// M249
	if (InputOwner->weaponArray[3] == true)
	{
		if (InputOwner->curM249Ammo > 0)
		{
			ProcessM249FireAnim();

			if (!InputOwner->IsLocallyControlled())
			{
				ProcessM249FireSimulatedProxy();
			}

			// 서버 로직 (핵심 프로세스 처리)
			if (InputOwner->HasAuthority())
			{
				ProcessM249Fire();
			}
		}
		else
		{
			InputOwner->AmmoDepleted();
		}
	}
}

bool UPlayerInputComponent::ServerRPCReload_Validate()
{
	return true;
}

void UPlayerInputComponent::ProcessRifleFireAnim()
{
	InputOwner->StopAnimMontage();
	InputOwner->PlayAnimMontage(InputOwner->RifleFireMontage, 1, FName("Fire"));
}

void UPlayerInputComponent::ProcessRifleFireLocal()
{
	UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->RifleFireSound);
	// 사격 카메라 셰이크 실행
	InputOwner->PC->PlayerCameraManager->StartCameraShake(InputOwner->rifleFireShake);
	if (InputOwner->FirstPersonCharacterMesh->IsVisible())
	{
		const FVector ParticleLoc = InputOwner->FirstPersonRifleComp->GetSocketLocation(FName("RifleFirePosition"));
		const UE::Math::TRotator<double> ParticleRot = InputOwner->FirstPersonRifleComp->GetSocketRotation(FName("RifleFirePosition"));
		const FTransform ParticleTrans = UKismetMathLibrary::MakeTransform(ParticleLoc, ParticleRot, FVector(0.4));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InputOwner->FirstPersonRifleFireParticle, ParticleTrans);
	}
	else
	{
		const FVector particleLoc = InputOwner->RifleComp->GetSocketLocation(FName("RifleFirePosition"));
		const UE::Math::TRotator<double> ParticleRot = InputOwner->RifleComp->GetSocketRotation(FName("RifleFirePosition"));
		const FTransform ParticleTrans = UKismetMathLibrary::MakeTransform(particleLoc, ParticleRot, FVector(0.4));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InputOwner->RifleFireParticle, ParticleTrans);
	}

	InputOwner->Stat->SetRecoilRate(InputOwner->weaponArray);
	InputOwner->AddControllerPitchInput(InputOwner->Stat->GetPitchRecoilRate());
	InputOwner->AddControllerYawInput(InputOwner->Stat->GetYawRecoilRate());
}

void UPlayerInputComponent::ProcessRifleFireSimulatedProxy() const
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->RifleFireSound, InputOwner->GetActorLocation());
	const FVector ParticleLoc = InputOwner->RifleComp->GetSocketLocation(FName("RifleFirePosition"));
	const UE::Math::TRotator<double> ParticleRot = InputOwner->RifleComp->GetSocketRotation(FName("RifleFirePosition"));
	const FTransform ParticleTrans = UKismetMathLibrary::MakeTransform(ParticleLoc, ParticleRot, FVector(0.4));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InputOwner->RifleFireParticle, ParticleTrans);
}

void UPlayerInputComponent::FireRelease()
{
	if (!InputOwner->IsZoomKeyPressed && InputOwner->weaponArray[1] == false && InputOwner->weaponArray[2] == false)
	{
		GetWorld()->GetTimerManager().SetTimer(InputOwner->ZoomFireHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			ZoomRelease(false);
		}), 0.5f, false);
	}
	InputOwner->EmptySoundBoolean = false;
}

void UPlayerInputComponent::ProcessRifleFire()
{
	if (InputOwner->curRifleAmmo > 0)
	{
		// Clamp를 통한 탄약 수 차감
		InputOwner->curRifleAmmo = FMath::Clamp(InputOwner->curRifleAmmo - 1, 0, 40);
		if (InputOwner->FirstPersonRifleComp->IsVisible())
		{
			InputOwner->RifleLineTraceStart = InputOwner->FirstPersonCamera->GetComponentLocation();
			InputOwner->RifleLineTraceEnd = InputOwner->RifleLineTraceStart + InputOwner->FirstPersonCamera->GetForwardVector() * 10000.0f;
		}
		else
		{
			InputOwner->RifleLineTraceStart = InputOwner->FollowCamera->GetComponentLocation();
			InputOwner->RifleLineTraceEnd = InputOwner->RifleLineTraceStart + InputOwner->FollowCamera->GetForwardVector() * 10000.0f;
		}
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InputOwner);
		// Perform Linetrace
		if (FHitResult RifleHitResult; GetWorld()->LineTraceSingleByChannel(RifleHitResult, InputOwner->RifleLineTraceStart, InputOwner->RifleLineTraceEnd, ECC_Visibility, Params))
		{
			if (!InputOwner->bSpacecraft)
			{
				// Player Character Casting
				// 플레이어 적중
				if (APlayerCharacter* Player = Cast<APlayerCharacter>(RifleHitResult.GetActor()))
				{
					if (RifleHitResult.BoneName == FName("head"))
					{
						InputOwner->OnPlayerHit(RifleHitResult, Player, true);
					}
					else
					{
						InputOwner->OnPlayerHit(RifleHitResult, Player, false);
					}
					return;
				}
			}
			// Enemy Casting
			if (AEnemy* Enemy = Cast<AEnemy>(RifleHitResult.GetActor()))
			{
				if (RifleHitResult.BoneName == FName("head"))
				{
					InputOwner->OnEnemyHit(RifleHitResult, Enemy, true);
				}
				else
				{
					InputOwner->OnEnemyHit(RifleHitResult, Enemy, false);
				}
				return;
			}
			// Reward Container Casting
			if (ARewardContainer* RewardContainer = Cast<ARewardContainer>(RifleHitResult.GetActor()))
			{
				InputOwner->OnContainerHit(RifleHitResult, RewardContainer);
			}
			// 지형지물에 적중
			else
			{
				InputOwner->OnGroundHit(RifleHitResult);
			}
		}
	}
}

void UPlayerInputComponent::ProcessSniperFire()
{
	if (InputOwner->curSniperAmmo > 0)
	{
		// Clamp를 통한 탄약 수 차감
		InputOwner->curSniperAmmo = FMath::Clamp(InputOwner->curSniperAmmo - 1, 0, 5);
		FVector StartLoc = InputOwner->FollowCamera->GetComponentLocation();
		FVector EndLoc = StartLoc + InputOwner->FollowCamera->GetForwardVector() * 10000.0f;
		FCollisionQueryParams params;
		params.AddIgnoredActor(InputOwner);
		// Perform Linetrace
		// 라인 트레이스가 적중했다면
		if (FHitResult SniperHitResult; GetWorld()->LineTraceSingleByChannel(SniperHitResult, StartLoc, EndLoc, ECC_Visibility, params))
		{
			if (!InputOwner->bSpacecraft)
			{
				// Player Character Casting
				// 플레이어 적중
				if (APlayerCharacter* Player = Cast<APlayerCharacter>(SniperHitResult.GetActor()))
				{
					if (SniperHitResult.BoneName == FName("head"))
					{
						InputOwner->OnPlayerHit(SniperHitResult, Player, true);
					}
					else
					{
						InputOwner->OnPlayerHit(SniperHitResult, Player, false);
					}
					return;
				}
			}
			// Enemy Casting
			if (AEnemy* Enemy = Cast<AEnemy>(SniperHitResult.GetActor()))
			{
				if (SniperHitResult.BoneName == FName("head"))
				{
					InputOwner->OnEnemyHit(SniperHitResult, Enemy, true);
				}
				else
				{
					InputOwner->OnEnemyHit(SniperHitResult, Enemy, false);
				}
				return;
			}
			// Reward Container Casting
			if (ARewardContainer* RewardContainer = Cast<ARewardContainer>(SniperHitResult.GetActor()))
			{
				InputOwner->OnContainerHit(SniperHitResult, RewardContainer);
			}
			else
			{
				InputOwner->OnGroundHit(SniperHitResult);
			}
		}
	}
}

void UPlayerInputComponent::ProcessSniperFireAnim()
{
	if (InputOwner->isZooming)
	{
		InputOwner->PlayAnimMontage(InputOwner->RifleFireMontage, 1, FName("SniperFire"));
	}
}

void UPlayerInputComponent::ProcessSniperFireLocal()
{
	UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->SniperFireSound);

	InputOwner->Stat->SetRecoilRate(InputOwner->weaponArray);
	InputOwner->AddControllerPitchInput(InputOwner->Stat->GetPitchRecoilRate());
	InputOwner->AddControllerYawInput(InputOwner->Stat->GetYawRecoilRate());

	if (InputOwner->isZooming)
	{
		const UE::Math::TVector<double> particleTrans = InputOwner->FollowCamera->GetComponentLocation() + InputOwner->FollowCamera->GetUpVector() * -70.0f;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InputOwner->SniperFireParticle, particleTrans);
		InputOwner->PC->PlayerCameraManager->StartCameraShake(InputOwner->sniperCameraShake);
	}
	else
	{
		FTransform particleTrans = InputOwner->SniperComp->GetSocketTransform(FName("SniperFirePosition"));
		particleTrans.SetScale3D(FVector(0.7));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InputOwner->SniperFireParticle, particleTrans);
		InputOwner->PC->PlayerCameraManager->StartCameraShake(InputOwner->sniperFireShake);
	}
}

void UPlayerInputComponent::ProcessSniperFireSimulatedProxy() const
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->SniperFireSound, InputOwner->GetActorLocation());
	FTransform ParticleTrans = InputOwner->SniperComp->GetSocketTransform(FName("SniperFirePosition"));
	ParticleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InputOwner->SniperFireParticle, ParticleTrans);
}

void UPlayerInputComponent::ProcessPistolFire()
{
	if (InputOwner->curPistolAmmo > 0)
	{
		// Clamp를 통한 탄약 수 차감
		InputOwner->curPistolAmmo = FMath::Clamp(InputOwner->curPistolAmmo - 1, 0, 8);
		if (InputOwner->FirstPersonPistolComp->IsVisible())
		{
			InputOwner->PistolLineTraceStart = InputOwner->FirstPersonCamera->GetComponentLocation();
			InputOwner->PistolLineTraceEnd = InputOwner->PistolLineTraceStart + InputOwner->FirstPersonCamera->GetForwardVector() * 10000.0f;
		}
		else
		{
			InputOwner->PistolLineTraceStart = InputOwner->FollowCamera->GetComponentLocation();
			InputOwner->PistolLineTraceEnd = InputOwner->PistolLineTraceStart + InputOwner->FollowCamera->GetForwardVector() * 10000.0f;
		}
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InputOwner);
		// Perform Linetrace
		if (FHitResult PistolHitResult; GetWorld()->LineTraceSingleByChannel(PistolHitResult, InputOwner->PistolLineTraceStart, InputOwner->PistolLineTraceEnd, ECC_Visibility, Params))
		{
			if (!InputOwner->bSpacecraft)
			{
				// Player Character Casting
				// 플레이어 적중
				if (APlayerCharacter* Player = Cast<APlayerCharacter>(PistolHitResult.GetActor()))
				{
					if (PistolHitResult.BoneName == FName("head"))
					{
						InputOwner->OnPlayerHit(PistolHitResult, Player, true);
					}
					else
					{
						InputOwner->OnPlayerHit(PistolHitResult, Player, false);
					}
					return;
				}
			}
			// Enemy Casting
			if (AEnemy* Enemy = Cast<AEnemy>(PistolHitResult.GetActor()))
			{
				if (PistolHitResult.BoneName == FName("head"))
				{
					InputOwner->OnEnemyHit(PistolHitResult, Enemy, true);
				}
				else
				{
					InputOwner->OnEnemyHit(PistolHitResult, Enemy, false);
				}
				return;
			}
			// Reward Container Casting
			if (ARewardContainer* RewardContainer = Cast<ARewardContainer>(PistolHitResult.GetActor()))
			{
				InputOwner->OnContainerHit(PistolHitResult, RewardContainer);
			}
			else
			{
				InputOwner->OnGroundHit(PistolHitResult);
			}
		}
	}
}

void UPlayerInputComponent::ProcessPistolFireAnim()
{
	if (InputOwner->isZooming)
	{
		InputOwner->PlayAnimMontage(InputOwner->FullBodyMontage, 1, FName("PistolZoomFire"));
	}
	else
	{
		InputOwner->PlayAnimMontage(InputOwner->FullBodyMontage, 1, FName("PistolFire"));
	}
}

void UPlayerInputComponent::ProcessPistolFireLocal()
{
	UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->PistolFireSound);
	InputOwner->PC->PlayerCameraManager->StartCameraShake(InputOwner->pistolFireShake);
	if (InputOwner->FirstPersonCharacterMesh->IsVisible())
	{
		if (const auto FirstAnimInstance = Cast<UFirstPersonPlayerAnim>(InputOwner->FirstPersonCharacterMesh->GetAnimInstance()))
		{
			FirstAnimInstance->Montage_Play(InputOwner->FirstPersonPistolFireMontage, 1);
		}
		const FVector ParticleLoc = InputOwner->FirstPersonPistolComp->GetSocketLocation(FName("PistolFirePosition"));
		const UE::Math::TRotator<double> ParticleRot = InputOwner->FirstPersonPistolComp->GetSocketRotation(FName("PistolFirePosition"));
		const FTransform particleTrans = UKismetMathLibrary::MakeTransform(ParticleLoc, ParticleRot, FVector(0.4));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InputOwner->FirstPersonRifleFireParticle, particleTrans);
	}
	else
	{
		FTransform ParticleTrans = InputOwner->PistolComp->GetSocketTransform(FName("PistolFirePosition"));
		ParticleTrans.SetScale3D(FVector(0.7));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InputOwner->PistolfireParticle, ParticleTrans);
	}

	InputOwner->Stat->SetRecoilRate(InputOwner->weaponArray);
	InputOwner->AddControllerPitchInput(InputOwner->Stat->GetPitchRecoilRate());
	InputOwner->AddControllerYawInput(InputOwner->Stat->GetYawRecoilRate());
}

void UPlayerInputComponent::ProcessPistolFireSimulatedProxy() const
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->PistolFireSound, InputOwner->GetActorLocation());
	FTransform ParticleTrans = InputOwner->PistolComp->GetSocketTransform(FName("PistolFirePosition"));
	ParticleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InputOwner->PistolfireParticle, ParticleTrans);
}

void UPlayerInputComponent::ProcessM249Fire()
{
	if (InputOwner->curM249Ammo > 0)
	{
		// Clamp를 통한 탄약 수 차감
		InputOwner->curM249Ammo = FMath::Clamp(InputOwner->curM249Ammo - 1, 0, 100);
		FVector StartLoc = InputOwner->FollowCamera->GetComponentLocation();
		FVector EndLoc = StartLoc + InputOwner->FollowCamera->GetForwardVector() * 10000.0f;
		FCollisionQueryParams params;
		params.AddIgnoredActor(InputOwner);
		// Perform Linetrace
		if (FHitResult M249HitResult; GetWorld()->LineTraceSingleByChannel(M249HitResult, StartLoc, EndLoc, ECC_Visibility, params))
		{
			if (!InputOwner->bSpacecraft)
			{
				// Player Character Casting
				// 플레이어 적중
				if (APlayerCharacter* Player = Cast<APlayerCharacter>(M249HitResult.GetActor()))
				{
					if (M249HitResult.BoneName == FName("head"))
					{
						InputOwner->OnPlayerHit(M249HitResult, Player, true);
					}
					else
					{
						InputOwner->OnPlayerHit(M249HitResult, Player, false);
					}
					return;
				}
			}
			// Enemy Casting
			if (AEnemy* Enemy = Cast<AEnemy>(M249HitResult.GetActor()))
			{
				if (M249HitResult.BoneName == FName("head"))
				{
					InputOwner->OnEnemyHit(M249HitResult, Enemy, true);
				}
				else
				{
					InputOwner->OnEnemyHit(M249HitResult, Enemy, false);
				}
				return;
			}
			// Reward Container Casting
			if (ARewardContainer* RewardContainer = Cast<ARewardContainer>(M249HitResult.GetActor()))
			{
				InputOwner->OnContainerHit(M249HitResult, RewardContainer);
			}
			else
			{
				InputOwner->OnGroundHit(M249HitResult);
			}
		}
	}
}

void UPlayerInputComponent::ProcessM249FireAnim()
{
	InputOwner->PlayAnimMontage(InputOwner->FullBodyMontage, 1, FName("RifleFire"));
}

void UPlayerInputComponent::ProcessM249FireLocal()
{
	FTransform ParticleTrans = InputOwner->M249Comp->GetSocketTransform(FName("M249FirePosition"));
	ParticleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InputOwner->RifleFireParticle, ParticleTrans);
	UGameplayStatics::PlaySound2D(GetWorld(), InputOwner->M249FireSound);
	InputOwner->PC->PlayerCameraManager->StartCameraShake(InputOwner->rifleFireShake);
	InputOwner->Stat->SetRecoilRate(InputOwner->weaponArray);
	InputOwner->AddControllerPitchInput(InputOwner->Stat->GetPitchRecoilRate());
	InputOwner->AddControllerYawInput(InputOwner->Stat->GetYawRecoilRate());
}

void UPlayerInputComponent::ProcessM249FireSimulatedProxy() const
{
	FTransform ParticleTrans = InputOwner->M249Comp->GetSocketTransform(FName("M249FirePosition"));
	ParticleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InputOwner->RifleFireParticle, ParticleTrans);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), InputOwner->M249FireSound, InputOwner->GetActorLocation());
}

void UPlayerInputComponent::RemoveAllWidgets()
{
	if (InputOwner->IsLocallyControlled())
	{
		if (IsValid(InputOwner->TradeWidgetUI) && InputOwner->TradeWidgetUI->IsInViewport()) InputOwner->TradeWidgetUI->RemoveFromParent();
		if (IsValid(InputOwner->MenuWidgetUI) && InputOwner->MenuWidgetUI->IsInViewport())InputOwner->MenuWidgetUI->RemoveFromParent();
		if (IsValid(InputOwner->levelSelectionUI) && InputOwner->levelSelectionUI->IsInViewport()) InputOwner->levelSelectionUI->RemoveFromParent();
		if (IsValid(InputOwner->quitWidgetUI) && InputOwner->quitWidgetUI->IsInViewport())InputOwner->quitWidgetUI->RemoveFromParent();
		if (IsValid(InputOwner->infoWidgetUI) && InputOwner->infoWidgetUI->IsInViewport())InputOwner->infoWidgetUI->RemoveFromParent();
		InputOwner->CloseTabWidget();
		if (IsValid(InputOwner->PC)) InputOwner->EnableInput(InputOwner->PC);
	}
}
