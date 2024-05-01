// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Eclipse/Item/ArmorActor.h"
#include "Eclipse/UI/CrosshairWidget.h"
#include "Eclipse/UI/DamageWidget.h"
#include "Eclipse/UI/DamageWidgetActor.h"
#include "Eclipse/Game/EclipseGameInstance.h"
#include "Eclipse/Player/EclipsePlayerController.h"
#include "Eclipse/Enemy/Enemy.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Eclipse/Item/GoggleActor.h"
#include "Eclipse/Item/HackingConsole.h"
#include "Eclipse/Item/HeadsetActor.h"
#include "Eclipse/Item/HelmetActor.h"
#include "Eclipse/UI/InformationWidget.h"
#include "Eclipse/UI/LevelSelection.h"
#include "Eclipse/Weapon/M249Actor.h"
#include "Eclipse/Item/M249MagActor.h"
#include "Eclipse/Item/MaskActor.h"
#include "Eclipse/Item/MedKitActor.h"
#include "Eclipse/Prop/MissionChecker.h"
#include "Eclipse/Animation/PlayerAnim.h"
#include "Eclipse/Weapon/RifleActor.h"
#include "Eclipse/Weapon/SniperActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Eclipse/Weapon/PistolActor.h"
#include "Eclipse/Item/PistolMagActor.h"
#include "Eclipse/Prop/QuitGameActor.h"
#include "Eclipse/UI/QuitWidget.h"
#include "Eclipse/Prop/RewardContainer.h"
#include "Eclipse/Item/RifleMagActor.h"
#include "Eclipse/Item/SniperMagActor.h"
#include "Eclipse/Prop/StageBoard.h"
#include "Eclipse/Prop/Stash.h"
#include "Eclipse/UI/WeaponInfoWidget.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Eclipse/Animation/FirstPersonPlayerAnim.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/Game/EclipseGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Game/EclipsePlayerState.h"
#include "Eclipse/Item/AdrenalineSyringe.h"
#include "Eclipse/Item/FirstAidKitActor.h"
#include "Eclipse/Item/MilitaryDevice.h"
#include "Eclipse/Item/MilitaryLaptop.h"
#include "Eclipse/Item/PoisonOfSpider.h"
#include "Eclipse/Prop/DeadPlayerContainer.h"
#include "Eclipse/Prop/Trader.h"
#include "Eclipse/UI/ExtractionCountdown.h"
#include "Eclipse/UI/MenuWidget.h"
#include "Eclipse/UI/TradeWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->AirControl = 0.35f;
	//GetCharacterMovement()->MaxWalkSpeed = CharacterWalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a FirstPersonCamera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonCamera->bUsePawnControlRotation = true;

	FirstPersonCharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonCharacterMesh"));
	FirstPersonCharacterMesh->SetupAttachment(FirstPersonCamera);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	// Configure character movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 150.0f, 0.0f); // ...at this rotation rate

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	SniperComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("sniperComp"));
	SniperComp->SetupAttachment(GetMesh(), FName("hand_r"));

	RifleComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("rifleComp"));
	RifleComp->SetupAttachment(GetMesh(), FName("hand_r"));

	FirstPersonRifleComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstPersonRifleComp"));
	FirstPersonRifleComp->SetupAttachment(FirstPersonCharacterMesh, FName("b_RightHand"));

	PistolComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("pistolComp"));
	PistolComp->SetupAttachment(GetMesh(), FName("hand_l"));

	FirstPersonPistolComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstPersonPistolComp"));
	FirstPersonPistolComp->SetupAttachment(FirstPersonCharacterMesh, FName("b_RightHand"));

	M249Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m249Comp"));
	M249Comp->SetupAttachment(GetMesh(), FName("hand_r"));

	GoggleSlot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GoggleSlot"));
	GoggleSlot->SetupAttachment(GetMesh(), FName("head"));

	HeadSetSlot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadSetSlot"));
	HeadSetSlot->SetupAttachment(GetMesh(), FName("head"));

	MaskSlot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MaskSlot"));
	MaskSlot->SetupAttachment(GetMesh(), FName("head"));

	HelmetSlot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HelmetSlot"));
	HelmetSlot->SetupAttachment(GetMesh(), FName("head"));

	ArmorSlot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmorSlot"));
	ArmorSlot->SetupAttachment(GetMesh(), FName("spine_03"));

	// Stat Component 
	Stat = CreateDefaultSubobject<UPlayerCharacterStatComponent>(TEXT("Stat"));

	SetReplicates(true);
	NetUpdateFrequency = 200.f;
	MinNetUpdateFrequency = 10.f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	this->SetActorEnableCollision(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->HideBoneByName(TEXT("bot_hand"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(TEXT("shotgun_base"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(TEXT("sniper_can_arm_01"), EPhysBodyOp::PBO_None);
	GetMesh()->SetVisibility(true);

	FirstPersonRifleComp->SetVisibility(false);
	FirstPersonPistolComp->SetVisibility(false);
	FirstPersonCharacterMesh->SetVisibility(false);
	FirstPersonCamera->SetActive(false);
	FollowCamera->SetActive(true);

	IsPlayerDeadImmediately = false;
	IsPlayerDead = false;

	// Casting
	const UGameInstance* GI = GetGameInstance();
	PC = Cast<AEclipsePlayerController>(GI->GetFirstLocalPlayerController());
	gm = Cast<AEclipseGameMode>(GetWorld()->GetAuthGameMode());
	gi = Cast<UEclipseGameInstance>(GetWorld()->GetGameInstance());
	animInstance = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());

	ApplyInventoryDataFromGameInstance();

	gi->IsWidgetOn = false;

	//Add Input Mapping Context
	if (PC)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			const FInputModeGameOnly InputModeData;
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			PC->SetInputMode(InputModeData);
			PC->SetShowMouseCursor(false);
		}
	}

	// Timeline Binding
	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &APlayerCharacter::SetZoomValue);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}

	// Widget Settings
	crosshairUI = CreateWidget<UCrosshairWidget>(GetWorld(), crosshairFactory);
	quitWidgetUI = CreateWidget<UQuitWidget>(GetWorld(), quitWidgetFactory);
	infoWidgetUI = CreateWidget<UWeaponInfoWidget>(GetWorld(), infoWidgetFactory);
	sniperScopeUI = CreateWidget<UUserWidget>(GetWorld(), sniperScopeFactory);
	damageWidgetUI = CreateWidget<UDamageWidget>(GetWorld(), damageWidgetUIFactory);
	bossHPUI = CreateWidget<UBossHPWidget>(GetWorld(), bossHPWidgetFactory);
	informationUI = CreateWidget<UInformationWidget>(GetWorld(), informationWidgetFactory);
	levelSelectionUI = CreateWidget<ULevelSelection>(GetWorld(), levelSelectionWidgetFactory);
	ExtractionCountdownUI = CreateWidget<UExtractionCountdown>(GetWorld(), ExtractionCountdownWidgetFactory);
	MenuWidgetUI = CreateWidget<UMenuWidget>(GetWorld(), MenuWidgetFactory);
	TradeWidgetUI = CreateWidget<UTradeWidget>(GetWorld(), TradeWidgetFactory);

	if (IsLocallyControlled())
	{
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
		if (!crosshairUI->IsInViewport())
		{
			crosshairUI->AddToViewport();
		}
		TradeWidgetUI->Construction(this);
		APlayerCameraManager* const CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
		CameraManager->StopCameraFade();
		CameraManager->StartCameraFade(1.0, 0, 8.0, FColor::Black, false, true);
	}

	ExtractionCountdownUI->ExtractionSuccessDele.AddUObject(this, &APlayerCharacter::ExtractionSuccess);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PlayerSpawnEmitter, GetActorLocation());

	if (PC)
	{
		//PC->SetAudioListenerOverride(GetMesh(), FVector::ZeroVector, FRotator::ZeroRotator);
		PC->EnableInput(PC);
	}

	// Hideout
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) == FString("Safe_House"))
	{
		if (animInstance)
		{
			animInstance->bArmed = false;
		}
		GetCharacterMovement()->MaxWalkSpeed = 360.f;
		bUsingRifle = false;
		bUsingSniper = false;
		bUsingPistol = false;
		bUsingM249 = false;
		equippedWeaponStringArray.Add(FString("Empty")); //0
		equippedWeaponStringArray.Add(FString("Empty")); //1

		// Weapon Visibility Settings
		RifleComp->SetVisibility(false);
		SniperComp->SetVisibility(false);
		PistolComp->SetVisibility(false);
		M249Comp->SetVisibility(false);

		// Gear Visibility Settings
		GoggleSlot->SetVisibility(false);
		HelmetSlot->SetVisibility(false);
		HeadSetSlot->SetVisibility(false);
		MaskSlot->SetVisibility(false);
		ArmorSlot->SetVisibility(false);
	}
	// Non Hideout
	else
	{
		if (animInstance)
		{
			animInstance->bArmed = true;
		}
		GetCharacterMovement()->MaxWalkSpeed = 360.f;
		bUsingRifle = true;
		bUsingSniper = false;
		bUsingPistol = false;
		bUsingM249 = false;

		equippedWeaponStringArray.Add(FString("Rifle")); //0
		equippedWeaponStringArray.Add(FString("Pistol")); //1

		// Weapon Visibility Settings
		RifleComp->SetVisibility(true);
		SniperComp->SetVisibility(false);
		PistolComp->SetVisibility(false);
		M249Comp->SetVisibility(false);

		// Gear Visibility Settings
		GoggleSlot->SetVisibility(false);
		HelmetSlot->SetVisibility(false);
		HeadSetSlot->SetVisibility(false);
		MaskSlot->SetVisibility(false);
		ArmorSlot->SetVisibility(false);
	}

	if (IsLocallyControlled())
	{
		if (informationUI)
		{
			FTimerHandle RespawnTimer;
			GetWorldTimerManager().SetTimer(RespawnTimer, FTimerDelegate::CreateLambda([this]()-> void
			{
				informationUI->owner = this;
				informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
				informationUI->BossCount->SetText(FText::AsNumber(BossCount));
				informationUI->ConsoleCount->SetText(FText::AsNumber(ConsoleCount));
				informationUI->UpdateAmmo();
				informationUI->UpdateAmmo_Secondary();
				if (!informationUI->IsInViewport())
				{
					informationUI->AddToViewport();
				}
			}), 0.5, false);
		}
	}

	// Set Weapon Array
	weaponArray.Add(bUsingRifle); //0
	weaponArray.Add(bUsingSniper); //1
	weaponArray.Add(bUsingPistol); //2
	weaponArray.Add(bUsingM249); //3	

	// Apply Inventory Cache [GameInstance]
	ApplyCachingValues();

	// Update Tab Widget Before Widget Constructor
	UpdateTabWidget();
}

// Called every frame
void APlayerCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timeline.TickTimeline(DeltaTime);

	WeaponDetectionLineTrace();
}

//////////////////////////////////////////////////////////////////////////
// Input
// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		//Zooming
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Started, this, &APlayerCharacter::ZoomInput);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Completed, this, &APlayerCharacter::ZoomReleaseInput);

		//Running
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &APlayerCharacter::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &APlayerCharacter::RunRelease);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Crouching);

		//Change Weapon
		EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ChangeWeapon);
		EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Started, this, &APlayerCharacter::DoorInteraction);

		//Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Fire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::FireRelease);

		//Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &APlayerCharacter::Reload);

		//Look Around
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnActionLookAroundPressed);
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Completed, this, &APlayerCharacter::OnActionLookAroundReleased);

		//Weapon Swap
		EnhancedInputComponent->BindAction(FirstWeaponSwapAction, ETriggerEvent::Started, this, &APlayerCharacter::SwapFirstWeapon);
		EnhancedInputComponent->BindAction(SecondWeaponSwapAction, ETriggerEvent::Started, this, &APlayerCharacter::SwapSecondWeapon);

		//Tab
		EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Started, this, &APlayerCharacter::Tab);

		//Open Menu
		EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Started, this, &APlayerCharacter::OpenMenu);

		//Q [Test]
		EnhancedInputComponent->BindAction(QAction, ETriggerEvent::Started, this, &APlayerCharacter::Q);
	}
}


void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (gi->IsWidgetOn)
	{
		return;
	}
	// input is a Vector2D
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::Jump()
{
	if (IsPlayerDeadImmediately)
	{
		return;
	}
	Super::Jump();
}

void APlayerCharacter::StopJumping()
{
	if (IsPlayerDeadImmediately)
	{
		return;
	}
	Super::StopJumping();
}

void APlayerCharacter::Zoom(const bool IsZoomInput)
{
	if (gi->IsWidgetOn || IsPlayerDeadImmediately)
	{
		return;
	}
	ZoomRPCServer(IsZoomInput);
}

void APlayerCharacter::ZoomRelease(const bool IsZoomInput)
{
	if (gi->IsWidgetOn || IsPlayerDeadImmediately)
	{
		return;
	}
	ZoomRPCReleaseServer(IsZoomInput);
}

void APlayerCharacter::ZoomInput()
{
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != FString("Safe_House"))
	{
		IsZoomKeyPressed = true;
		Zoom(true);
	}
}

void APlayerCharacter::ZoomReleaseInput()
{
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != FString("Safe_House"))
	{
		IsZoomKeyPressed = false;
		ZoomRelease(true);
	}
}

void APlayerCharacter::ZoomRPCMulticast_Implementation(const bool IsZoomInput)
{
	// Zooming Boolean
	isZooming = true;
	UPlayerAnim* const AnimInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if (AnimInst)
	{
		AnimInst->bZooming = true;
	}
	CharacterWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = 180.f;

	// is using rifle
	if (weaponArray[0] == true)
	{
		AnimInst->bRifleZooming = true;
		if (IsLocallyControlled())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), zoomSound);
			if (IsZoomInput)
			{
				SetFirstPersonModeRifle(true);
				return;
			}
			Timeline.PlayFromStart();
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), zoomSound, GetActorLocation());
		}
	}
	// is using sniper
	else if (weaponArray[1] == true)
	{
		if (IsLocallyControlled())
		{
			isSniperZooming = true;
			Timeline.PlayFromStart();

			crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
			APlayerCameraManager* const cameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
			cameraManager->StartCameraFade(1.0, 0.1, 3.0, FColor::Black, false, true);
			PC->PlayerCameraManager->StartCameraShake(sniperZoomingShake);
			UGameplayStatics::PlaySound2D(GetWorld(), SniperZoomSound);
			// 카메라 줌 러프 타임라인 재생
			sniperScopeUI->AddToViewport();
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SniperZoomSound, GetActorLocation());
		}
		if (AnimInst)
		{
			AnimInst->bRifleZooming = true;
		}
	}
	else if (weaponArray[2] == true)
	{
		if (IsLocallyControlled())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), zoomSound);
			if (IsZoomInput)
			{
				SetFirstPersonModePistol(true);
				return;
			}
			Timeline.PlayFromStart();
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), zoomSound, GetActorLocation());
		}
	}
	else if (weaponArray[3] == true)
	{
		if (IsLocallyControlled())
		{
			Timeline.PlayFromStart();

			UGameplayStatics::PlaySound2D(GetWorld(), zoomSound);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), zoomSound, GetActorLocation());
		}
		if (AnimInst)
		{
			AnimInst->bM249Zooming = true;
		}
	}
}

void APlayerCharacter::ZoomRPCServer_Implementation(const bool IsZoomInput)
{
	ZoomRPCMulticast(IsZoomInput);
}

bool APlayerCharacter::ZoomRPCServer_Validate(const bool IsZoomInput)
{
	return true;
}


void APlayerCharacter::ZoomRPCReleaseServer_Implementation(const bool IsZoomInput)
{
	ZoomRPCReleaseMulticast(IsZoomInput);
}

bool APlayerCharacter::ZoomRPCReleaseServer_Validate(const bool IsZoomInput)
{
	return true;
}

void APlayerCharacter::ZoomRPCReleaseMulticast_Implementation(const bool IsZoomInput)
{
	// Zooming Boolean
	isZooming = false;
	GetCharacterMovement()->MaxWalkSpeed = CharacterWalkSpeed;
	UPlayerAnim* const AnimInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if (AnimInst)
	{
		AnimInst->bZooming = false;
	}
	if (weaponArray[0] == true)
	{
		AnimInst->bRifleZooming = false;
		if (IsLocallyControlled())
		{
			if (IsZoomInput)
			{
				SetFirstPersonModeRifle(false);
				return;
			}
			Timeline.ReverseFromEnd();
		}
	}
	else if (weaponArray[1] == true)
	{
		if (IsLocallyControlled())
		{
			isSniperZooming = false;
			PC->PlayerCameraManager->StopAllCameraShakes();
			if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(UpperOnlyMontage))
			{
				StopAnimMontage();
			}
			sniperScopeUI->RemoveFromParent();
			crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Visible);
			SniperZoomBool = false;
			SniperZoomOutBool = false;
			Timeline.ReverseFromEnd();
		}
		if (AnimInst)
		{
			AnimInst->bRifleZooming = false;
		}
	}
	else if (weaponArray[2] == true)
	{
		if (IsLocallyControlled())
		{
			if (IsZoomInput)
			{
				SetFirstPersonModePistol(false);
				return;
			}
			Timeline.ReverseFromEnd();
		}
	}
	else if (weaponArray[3] == true)
	{
		if (AnimInst)
		{
			AnimInst->bM249Zooming = false;
		}
		Timeline.ReverseFromEnd();
	}
	else
	{
		Timeline.ReverseFromEnd();
	}
}


void APlayerCharacter::Run()
{
	RunRPCServer();
}

void APlayerCharacter::RunRelease()
{
	RunRPCReleaseServer();
}

void APlayerCharacter::RunRPCMulticast_Implementation()
{
	if (isSniperZooming)
	{
		if (!isSniperZoomed)
		{
			GetWorldTimerManager().ClearTimer(SniperZoomOutHandle);
			SniperZoomBool = true;
			SniperZoomOutBool = false;
			Timeline.Stop();
			FollowCamera->SetFieldOfView(40);
			Timeline.PlayFromStart();
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SniperZoomInSound, GetActorLocation());
			isSniperZoomed = true;
		}
		else if (isSniperZoomed)
		{
			GetWorldTimerManager().ClearTimer(SniperZoomHandle);
			SniperZoomOutBool = true;
			SniperZoomBool = false;
			Timeline.Stop();
			FollowCamera->SetFieldOfView(20);
			Timeline.PlayFromStart();
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SniperZoomOutSound, GetActorLocation());
			isSniperZoomed = false;
		}
		return;
	}
	if (isZooming)
	{
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed = 520.f;
}

void APlayerCharacter::RunRPCServer_Implementation()
{
	RunRPCMulticast();
}

bool APlayerCharacter::RunRPCServer_Validate()
{
	return true;
}


void APlayerCharacter::RunRPCReleaseServer_Implementation()
{
	RunRPCReleaseMulticast();
}

bool APlayerCharacter::RunRPCReleaseServer_Validate()
{
	return true;
}

void APlayerCharacter::RunRPCReleaseMulticast_Implementation()
{
	if (isZooming)
	{
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed = 360.f;
}


void APlayerCharacter::OnActionLookAroundPressed()
{
	bUseControllerRotationYaw = false;
}

void APlayerCharacter::OnActionLookAroundReleased()
{
	bUseControllerRotationYaw = true;
}


void APlayerCharacter::SwapFirstWeapon()
{
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != FString("Safe_House"))
	{
		SwapFirstWeaponRPCServer();
	}
}

void APlayerCharacter::SwapFirstWeaponRPCServer_Implementation()
{
	SwapFirstWeaponRPCMulticast();
}

bool APlayerCharacter::SwapFirstWeaponRPCServer_Validate()
{
	return true;
}

void APlayerCharacter::SwapFirstWeaponRPCMulticast_Implementation()
{
	if (curWeaponSlotNumber == 1 || isSniperZooming)
	{
		return;
	}
	if (const UPlayerAnim* AnimInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance()))
	{
		if (AnimInst->IsAnyMontagePlaying())
		{
			return;
		}
	}
	if (IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SwapSound);
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwapSound);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SwapSound, GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSwapSound, GetActorLocation());
	}
	curWeaponSlotNumber = 1;

	if (equippedWeaponStringArray[0] == FString("Rifle"))
	{
		animInstance->bPistol = false;
		PlayAnimMontage(UpperOnlyMontage, 1, FName("WeaponEquip"));
		// Visibility 설정
		RifleComp->SetVisibility(true);
		SniperComp->SetVisibility(false);
		PistolComp->SetVisibility(false);
		M249Comp->SetVisibility(false);
		// 무기 배열 설정
		weaponArray[0] = true;
		weaponArray[1] = false;
		weaponArray[2] = false;
		weaponArray[3] = false;

		if (IsLocallyControlled())
		{
			informationUI->PlayAnimation(informationUI->WeaponSwap);
		}
		informationUI->UpdateAmmo_Secondary();
	}
	else if (equippedWeaponStringArray[0] == FString("Sniper"))
	{
		animInstance->bPistol = false;
		PlayAnimMontage(UpperOnlyMontage, 1, FName("WeaponEquip"));
		// Visibility 설정
		RifleComp->SetVisibility(false);
		SniperComp->SetVisibility(true);
		PistolComp->SetVisibility(false);
		M249Comp->SetVisibility(false);
		// 무기 배열 설정
		weaponArray[0] = false;
		weaponArray[1] = true;
		weaponArray[2] = false;
		weaponArray[3] = false;

		if (IsLocallyControlled())
		{
			informationUI->PlayAnimation(informationUI->WeaponSwap);
		}
		informationUI->UpdateAmmo_Secondary();
	}
	else if (equippedWeaponStringArray[0] == FString("Pistol"))
	{
		animInstance->bPistol = true;
		PlayAnimMontage(UpperOnlyMontage, 1, FName("PistolEquip"));
		// Visibility 설정
		RifleComp->SetVisibility(false);
		SniperComp->SetVisibility(false);
		PistolComp->SetVisibility(true);
		M249Comp->SetVisibility(false);
		// 무기 배열 설정
		weaponArray[0] = false;
		weaponArray[1] = false;
		weaponArray[2] = true;
		weaponArray[3] = false;

		if (IsLocallyControlled())
		{
			informationUI->PlayAnimation(informationUI->WeaponSwap);
		}
		informationUI->UpdateAmmo_Secondary();
	}
	else if (equippedWeaponStringArray[0] == FString("M249"))
	{
		animInstance->bPistol = false;
		PlayAnimMontage(UpperOnlyMontage, 1, FName("WeaponEquip"));
		// Visibility 설정
		RifleComp->SetVisibility(false);
		SniperComp->SetVisibility(false);
		PistolComp->SetVisibility(false);
		M249Comp->SetVisibility(true);
		// 무기 배열 설정
		weaponArray[0] = false;
		weaponArray[1] = false;
		weaponArray[2] = false;
		weaponArray[3] = true;

		if (IsLocallyControlled())
		{
			informationUI->PlayAnimation(informationUI->WeaponSwap);
		}
		informationUI->UpdateAmmo_Secondary();
	}
}

void APlayerCharacter::SwapSecondWeapon()
{
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != FString("Safe_House"))
	{
		SwapSecondWeaponRPCServer();
	}
}

void APlayerCharacter::SwapSecondWeaponRPCServer_Implementation()
{
	SwapSecondWeaponRPCMulticast();
}

bool APlayerCharacter::SwapSecondWeaponRPCServer_Validate()
{
	return true;
}

void APlayerCharacter::SwapSecondWeaponRPCMulticast_Implementation()
{
	if (curWeaponSlotNumber == 2 || isSniperZooming)
	{
		return;
	}
	if (const UPlayerAnim* AnimInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance()))
	{
		if (AnimInst->IsAnyMontagePlaying())
		{
			return;
		}
	}
	if (IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SwapSound);
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwapSound);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SwapSound, GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSwapSound, GetActorLocation());
	}

	curWeaponSlotNumber = 2;

	if (equippedWeaponStringArray[1] == FString("Rifle"))
	{
		animInstance->bPistol = false;
		PlayAnimMontage(UpperOnlyMontage, 1, FName("WeaponEquip"));
		// Visibility 설정
		RifleComp->SetVisibility(true);
		SniperComp->SetVisibility(false);
		PistolComp->SetVisibility(false);
		M249Comp->SetVisibility(false);
		// 무기 배열 설정
		weaponArray[0] = true;
		weaponArray[1] = false;
		weaponArray[2] = false;
		weaponArray[3] = false;

		if (IsLocallyControlled())
		{
			informationUI->PlayAnimation(informationUI->WeaponSwap);
		}
		informationUI->UpdateAmmo_Secondary();
	}
	else if (equippedWeaponStringArray[1] == FString("Sniper"))
	{
		animInstance->bPistol = false;
		PlayAnimMontage(UpperOnlyMontage, 1, FName("WeaponEquip"));
		// Visibility 설정
		RifleComp->SetVisibility(false);
		SniperComp->SetVisibility(true);
		PistolComp->SetVisibility(false);
		M249Comp->SetVisibility(false);
		// 무기 배열 설정
		weaponArray[0] = false;
		weaponArray[1] = true;
		weaponArray[2] = false;
		weaponArray[3] = false;

		if (IsLocallyControlled())
		{
			informationUI->PlayAnimation(informationUI->WeaponSwap);
		}
		informationUI->UpdateAmmo_Secondary();
	}
	else if (equippedWeaponStringArray[1] == FString("Pistol"))
	{
		animInstance->bPistol = true;
		PlayAnimMontage(UpperOnlyMontage, 1, FName("PistolEquip"));
		// Visibility 설정
		RifleComp->SetVisibility(false);
		SniperComp->SetVisibility(false);
		PistolComp->SetVisibility(true);
		M249Comp->SetVisibility(false);
		// 무기 배열 설정
		weaponArray[0] = false;
		weaponArray[1] = false;
		weaponArray[2] = true;
		weaponArray[3] = false;

		if (IsLocallyControlled())
		{
			informationUI->PlayAnimation(informationUI->WeaponSwap);
		}
		informationUI->UpdateAmmo_Secondary();
	}
	else if (equippedWeaponStringArray[1] == FString("M249"))
	{
		animInstance->bPistol = false;
		PlayAnimMontage(UpperOnlyMontage, 1, FName("WeaponEquip"));
		// Visibility 설정
		RifleComp->SetVisibility(false);
		SniperComp->SetVisibility(false);
		PistolComp->SetVisibility(false);
		M249Comp->SetVisibility(true);
		// 무기 배열 설정
		weaponArray[0] = false;
		weaponArray[1] = false;
		weaponArray[2] = false;
		weaponArray[3] = true;

		if (IsLocallyControlled())
		{
			informationUI->PlayAnimation(informationUI->WeaponSwap);
		}
		informationUI->UpdateAmmo_Secondary();
	}
}


void APlayerCharacter::OnPlayerHit(const FHitResult& HitResult, APlayerCharacter* HitCharacter, bool IsHeadshot)
{
	if (HitCharacter->Stat->GetCurrentHp() > 0)
	{
		OnPlayerHitRPCServer(HitResult, HitCharacter, IsHeadshot);
	}
}


void APlayerCharacter::OnPlayerHitRPCServer_Implementation(const FHitResult& HitResult, APlayerCharacter* HitCharacter, bool IsHeadshot)
{
	OnPlayerHitRPCMulticast(HitResult, HitCharacter, IsHeadshot);
}

bool APlayerCharacter::OnPlayerHitRPCServer_Validate(const FHitResult& HitResult, APlayerCharacter* HitCharacter, bool IsHeadshot)
{
	return true;
}

void APlayerCharacter::OnPlayerHitRPCMulticast_Implementation(const FHitResult& HitResult, APlayerCharacter* HitCharacter, bool IsHeadshot)
{
	if (HasAuthority())
	{
		if (IsHeadshot)
		{
			DamageAmount = Stat->GetAttackDamage(weaponArray, true);
			HitCharacter->Damaged(DamageAmount * 2, this);
			Stat->AccumulatedDamageToPlayer += DamageAmount * 2;
		}
		else
		{
			DamageAmount = Stat->GetAttackDamage(weaponArray, true);
			HitCharacter->Damaged(DamageAmount, this);
			Stat->AccumulatedDamageToPlayer += DamageAmount;
		}
	}
	if (IsLocallyControlled())
	{
		const FRotator hitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		if (IsHeadshot)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), BulletHeadHitSound);
			// 적중 위젯 애니메이션 재생
			crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
			// 데미지 위젯에 피해 값과 적 위치벡터 할당
			SetDamageWidget(DamageAmount * 2, HitResult.Location, false, FLinearColor::Yellow);
			// 적중 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticle, HitResult.Location, hitRot, FVector(1.f));
		}
		else
		{
			UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
			// 적중 위젯 애니메이션 재생
			crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
			// 데미지 위젯에 피해 값과 적 위치벡터 할당
			SetDamageWidget(DamageAmount, HitResult.Location, false, FLinearColor::White);
			// 적중 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticle, HitResult.Location, hitRot, FVector(1.f));
		}
	}
	else
	{
		const FRotator hitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		if (IsHeadshot)
		{
			// 적중 사운드 재생
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHeadHitSound, HitResult.Location);
			// 적중 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticle, HitResult.Location, hitRot, FVector(1.7f));
		}
		else
		{
			// 적중 사운드 재생
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, HitResult.Location);
			// 적중 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticle, HitResult.Location, hitRot, FVector(1.f));
		}
	}
}

void APlayerCharacter::OnPlayerKill()
{
	OnPlayerKillRPCServer();
}

void APlayerCharacter::OnPlayerKillRPCServer_Implementation()
{
	OnPlayerKillRPCMulticast();
}

bool APlayerCharacter::OnPlayerKillRPCServer_Validate()
{
	return true;
}

void APlayerCharacter::OnPlayerKillRPCMulticast_Implementation()
{
	if (IsLocallyControlled())
	{
		crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
		UGameplayStatics::PlaySound2D(GetWorld(), PlayerKillSound, 1, 1, 0.25);
	}
}


void APlayerCharacter::OnEnemyHit(const FHitResult& HitResult, AEnemy* HitEnemy, bool IsHeadshot)
{
	if (HitEnemy->EnemyStat->GetCurrentHp() > 0)
	{
		OnEnemyHitRPCServer(HitResult, HitEnemy, IsHeadshot);
	}
}

void APlayerCharacter::OnEnemyKill()
{
	OnEnemyKillRPCServer();
}

void APlayerCharacter::OnEnemyKillRPCServer_Implementation()
{
	OnEnemyKillRPCMulticast();
}

bool APlayerCharacter::OnEnemyKillRPCServer_Validate()
{
	return true;
}

void APlayerCharacter::OnEnemyKillRPCMulticast_Implementation()
{
	if (HasAuthority())
	{
		if (weaponArray[0]) maxRifleAmmo += 20;
		else if (weaponArray[1]) maxSniperAmmo += 4;
		else if (weaponArray[2]) maxPistolAmmo += 6;
		else if (weaponArray[3]) maxM249Ammo += 30;
	}
	if (IsLocallyControlled())
	{
		crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
		UGameplayStatics::PlaySound2D(GetWorld(), KillSound);
		informationUI->ChargeAmmunitionInfoWidget();
		informationUI->PlayAnimation(informationUI->ChargeAmmunition);
		informationUI->UpdateAmmo_Secondary();
		FTimerHandle AmmoPickHandle;
		GetWorldTimerManager().SetTimer(AmmoPickHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			UGameplayStatics::PlaySound2D(GetWorld(), AmmoPickupSound);
		}), 1.f, false);
	}
}


void APlayerCharacter::OnEnemyHitRPCServer_Implementation(const FHitResult& HitResult, AEnemy* HitEnemy, bool IsHeadshot)
{
	OnEnemyHitRPCMulticast(HitResult, HitEnemy, IsHeadshot);
}

bool APlayerCharacter::OnEnemyHitRPCServer_Validate(const FHitResult& HitResult, AEnemy* HitEnemy, bool IsHeadshot)
{
	return true;
}

void APlayerCharacter::OnEnemyHitRPCMulticast_Implementation(const FHitResult& HitResult, AEnemy* HitEnemy, const bool IsHeadshot)
{
	if (HasAuthority())
	{
		if (IsHeadshot)
		{
			DamageAmount = Stat->GetAttackDamage(weaponArray, false);
			HitEnemy->Damaged(DamageAmount * 2, this);
			Stat->AccumulatedDamageToEnemy += DamageAmount * 2;
		}
		else
		{
			DamageAmount = Stat->GetAttackDamage(weaponArray, false);
			HitEnemy->Damaged(DamageAmount, this);
			Stat->AccumulatedDamageToEnemy += DamageAmount;
		}
	}
	if (IsLocallyControlled())
	{
		const FRotator HitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		if (HitEnemy->EnemyStat->IsStunned)
		{
			if (IsHeadshot)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), BulletHeadHitSound);
				// 적중 위젯 애니메이션 재생
				crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
				// 데미지 위젯에 피해 값과 적 위치벡터 할당
				SetDamageWidget(DamageAmount * 4, HitResult.Location, false, FLinearColor::Red);
				// 적중 파티클 스폰
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(2.f));
			}
			else
			{
				UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
				// 적중 위젯 애니메이션 재생
				crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
				// 데미지 위젯에 피해 값과 적 위치벡터 할당
				SetDamageWidget(DamageAmount * 2, HitResult.Location, false, FLinearColor::Red);
				// 적중 파티클 스폰
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(1.f));
			}
		}
		else
		{
			if (HitEnemy->EnemyStat->IsShieldBroken)
			{
				if (IsHeadshot)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), BulletHeadHitSound);
					// 적중 위젯 애니메이션 재생
					crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
					// 데미지 위젯에 피해 값과 적 위치벡터 할당
					SetDamageWidget(DamageAmount * 2, HitResult.Location, false, FLinearColor::Yellow);
					// 적중 파티클 스폰
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(2.f));
				}
				else
				{
					UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
					// 적중 위젯 애니메이션 재생
					crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
					// 데미지 위젯에 피해 값과 적 위치벡터 할당
					SetDamageWidget(DamageAmount, HitResult.Location, false, FLinearColor::White);
					// 적중 파티클 스폰
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(1.f));
				}
			}
			else
			{
				if (IsHeadshot)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), BulletHeadHitSound);
					// 적중 위젯 애니메이션 재생
					crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
					// 데미지 위젯에 피해 값과 적 위치벡터 할당
					SetDamageWidget(DamageAmount * 0.1f, HitResult.Location, true, FLinearColor::Gray);
					// 적중 파티클 스폰
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(2.f));
				}
				else
				{
					UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
					// 적중 위젯 애니메이션 재생
					crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
					// 데미지 위젯에 피해 값과 적 위치벡터 할당
					SetDamageWidget(DamageAmount * 0.05f, HitResult.Location, true, FLinearColor::Gray);
					// 적중 파티클 스폰
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(1.f));
				}
			}
		}
	}
	else
	{
		const FRotator HitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		if (IsHeadshot)
		{
			// 적중 사운드 재생
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHeadHitSound, HitResult.Location);
			// 적중 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(2.f));
		}
		else
		{
			// 적중 사운드 재생
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, HitResult.Location);
			// 적중 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(1.f));
		}
	}
}


void APlayerCharacter::OnContainerHit(const FHitResult& HitResult, ARewardContainer* HitContainer)
{
	if (!HitContainer->IsBoxDestroyed)
	{
		OnContainerHitRPCServer(HitResult, HitContainer);
	}
}

void APlayerCharacter::OnContainerHitRPCServer_Implementation(const FHitResult& HitResult, ARewardContainer* HitContainer)
{
	OnContainerHitRPCMulticast(HitResult, HitContainer);
}

bool APlayerCharacter::OnContainerHitRPCServer_Validate(const FHitResult& HitResult, ARewardContainer* HitContainer)
{
	return true;
}

void APlayerCharacter::OnContainerHitRPCMulticast_Implementation(const FHitResult& HitResult, ARewardContainer* HitContainer)
{
	if (HasAuthority())
	{
		if (HitContainer->curBoxHP <= 1)
		{
			HitContainer->BoxDestroyed();
		}
		else
		{
			HitContainer->curBoxHP = FMath::Clamp(HitContainer->curBoxHP - 1, 0, 5);
		}
	}
	if (IsLocallyControlled())
	{
		crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
		UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, HitContainer->GetActorLocation());
	}
	const FRotator HitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(1.f));
}

void APlayerCharacter::OnGroundHit(const FHitResult& HitResult)
{
	OnGroundHitRPCServer(HitResult);
}

void APlayerCharacter::OnGroundHitRPCServer_Implementation(const FHitResult& HitResult)
{
	OnGroundHitRPCMulticast(HitResult);
}

bool APlayerCharacter::OnGroundHitRPCServer_Validate(const FHitResult& HitResult)
{
	return true;
}

void APlayerCharacter::OnGroundHitRPCMulticast_Implementation(const FHitResult& HitResult)
{
	if (IsLocallyControlled())
	{
		const FRotator DecalRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		const FVector_NetQuantize DecalLoc = HitResult.Location;
		const FTransform DecalTrans = UKismetMathLibrary::MakeTransform(DecalLoc, DecalRot);
		GetWorld()->SpawnActor<AActor>(ShotDecalFactory, DecalTrans);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, DecalLoc, DecalRot + FRotator(-90, 0, 0), FVector(0.5f));
	}
	else
	{
		const FRotator DecalRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		const FVector_NetQuantize DecalLoc = HitResult.Location;
		const FTransform DecalTrans = UKismetMathLibrary::MakeTransform(DecalLoc, DecalRot);
		GetWorld()->SpawnActor<AActor>(ShotDecalFactory, DecalTrans);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, DecalLoc, DecalRot + FRotator(-90, 0, 0), FVector(0.5f));
	}
}

int32 APlayerCharacter::GenerateRandomDamage(const float InDamage) const
{
	const double DoubleRandDamage = FMath::FRandRange(InDamage * 0.8, InDamage * 1.2);
	const int32 RoundedRandDamage = FMath::RoundHalfToEven(DoubleRandDamage);
	return RoundedRandDamage;
}


void APlayerCharacter::Tab()
{
}

void APlayerCharacter::OpenMenu()
{
	if (MenuWidgetUI && !MenuWidgetUI->IsInViewport())
	{
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, MenuWidgetUI);
		PC->SetShowMouseCursor(true);
		MenuWidgetUI->AddToViewport();
	}
	else
	{
		MenuWidgetUI->CloseWidgetFunc();
	}
}

void APlayerCharacter::Q()
{
}


void APlayerCharacter::WeaponDetectionLineTrace()
{
	if (IsLocallyControlled())
	{
		// 스나이퍼 줌 도중 교체 불가
		if (isSniperZooming || bEnding)
		{
			return;
		}
		if (infoWidgetUI == nullptr)
		{
			return;
		}
		FVector StartLoc = FollowCamera->GetComponentLocation();
		FVector EndLoc = StartLoc + FollowCamera->GetForwardVector() * 500.0f;
		// 무기 액터 탐지 라인 트레이스
		if (FHitResult ActorHitResult; GetWorld()->LineTraceSingleByChannel(ActorHitResult, StartLoc, EndLoc, ECC_Visibility))
		{
			// 무기 액터 캐스팅
			rifleActor = Cast<ARifleActor>(ActorHitResult.GetActor());
			sniperActor = Cast<ASniperActor>(ActorHitResult.GetActor());
			pistolActor = Cast<APistolActor>(ActorHitResult.GetActor());
			m249Actor = Cast<AM249Actor>(ActorHitResult.GetActor());
			MissionChecker = Cast<AMissionChecker>(ActorHitResult.GetActor());
			PickableItemActor = Cast<APickableActor>(ActorHitResult.GetActor());
			StageBoard = Cast<AStageBoard>(ActorHitResult.GetActor());
			Stash = Cast<AStash>(ActorHitResult.GetActor());
			Trader = Cast<ATrader>(ActorHitResult.GetActor());
			QuitGameActor = Cast<AQuitGameActor>(ActorHitResult.GetActor());
			DeadPlayerContainer = Cast<ADeadPlayerContainer>(ActorHitResult.GetActor());

			// 라이플 탐지
			if (rifleActor)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					rifleActor->weaponMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(0);
					// Radial Slider Value 초기화
					infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					infoWidgetUI->AddToViewport();
				}
			}
			else if (sniperActor)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					sniperActor->weaponMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(1);
					// Radial Slider Value 초기화
					infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					infoWidgetUI->AddToViewport();
				}
			}
			else if (pistolActor)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					pistolActor->weaponMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(2);
					// Radial Slider Value 초기화
					infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					infoWidgetUI->AddToViewport();
				}
			}
			else if (m249Actor)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					m249Actor->weaponMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(3);
					// Radial Slider Value 초기화
					infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					infoWidgetUI->AddToViewport();
				}
			}
			else if (PickableItemActor)
			{
				HackingConsole = Cast<AHackingConsole>(PickableItemActor);
				RifleMagActor = Cast<ARifleMagActor>(PickableItemActor);
				SniperMagActor = Cast<ASniperMagActor>(PickableItemActor);
				PistolMagActor = Cast<APistolMagActor>(PickableItemActor);
				M249MagActor = Cast<AM249MagActor>(PickableItemActor);
				GoggleActor = Cast<AGoggleActor>(PickableItemActor);
				MaskActor = Cast<AMaskActor>(PickableItemActor);
				HelmetActor = Cast<AHelmetActor>(PickableItemActor);
				HeadsetActor = Cast<AHeadsetActor>(PickableItemActor);
				ArmorActor = Cast<AArmorActor>(PickableItemActor);
				MedKitActor = Cast<AMedKitActor>(PickableItemActor);
				MilitaryLaptop = Cast<AMilitaryLaptop>(PickableItemActor);
				MilitaryDevice = Cast<AMilitaryDevice>(PickableItemActor);
				AdrenalineSyringe = Cast<AAdrenalineSyringe>(PickableItemActor);
				PoisonOfSpider = Cast<APoisonOfSpider>(PickableItemActor);
				FirstAidKitActor = Cast<AFirstAidKitActor>(PickableItemActor);
				if (RifleMagActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						RifleMagActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(6);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (SniperMagActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						SniperMagActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(7);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (PistolMagActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						PistolMagActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(8);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (M249MagActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						M249MagActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(9);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (GoggleActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						GoggleActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(10);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (HelmetActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						HelmetActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(11);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (HeadsetActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						HeadsetActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(12);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (MaskActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						MaskActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(13);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (ArmorActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						ArmorActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(14);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (MedKitActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						MedKitActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(15);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (FirstAidKitActor)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						FirstAidKitActor->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(20);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (AdrenalineSyringe)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						AdrenalineSyringe->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(23);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (PoisonOfSpider)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						PoisonOfSpider->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(24);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (MilitaryLaptop)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						MilitaryLaptop->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(21);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (MilitaryDevice)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						MilitaryDevice->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(22);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
				else if (HackingConsole)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						HackingConsole->RootMesh->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(4);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
				}
			}

			else if (MissionChecker)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					MissionChecker->checkerMesh->SetRenderCustomDepth(true);
					if (BossCount >= 1 && GuardianCount >= 7)
					{
						infoWidgetUI->MissionCheck1->SetText(FText::FromString("Success"));
					}
					if (ConsoleCount >= 5)
					{
						infoWidgetUI->MissionCheck2->SetText(FText::FromString("Success"));
					}
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(5);
					// Radial Slider Value 초기화
					infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					infoWidgetUI->AddToViewport();
				}
			}
			else if (StageBoard)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					StageBoard->boardMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(16);
					// Radial Slider Value 초기화
					infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					infoWidgetUI->AddToViewport();
				}
			}
			else if (Stash)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					Stash->stashMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(17);
					// Radial Slider Value 초기화
					infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					infoWidgetUI->AddToViewport();
				}
			}
			else if (Trader)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					Trader->TraderCharacterMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(25);
					// Radial Slider Value 초기화
					infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					infoWidgetUI->AddToViewport();
				}
			}
			else if (QuitGameActor)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					QuitGameActor->quitGameMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(18);
					// Radial Slider Value 초기화
					infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					infoWidgetUI->AddToViewport();
				}
			}
			else if (DeadPlayerContainer)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					// Render Custom Depth 활용한 무기 액터 외곽선 활성화
					DeadPlayerContainer->DeadBodyMesh->SetRenderCustomDepth(true);
					// Widget Switcher 이용한 무기 정보 위젯 스위칭
					infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(19);
					// Radial Slider Value 초기화
					infoWidgetUI->weaponHoldPercent = 0;
					// Weapon Info Widget 뷰포트에 배치
					infoWidgetUI->AddToViewport();
				}
			}
			else
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == true)
				{
					TickOverlapBoolean = false;
					// 무기 액터 정보 위젯 파괴
					infoWidgetUI->RemoveFromParent();
					// 중심점
					FVector Center = this->GetActorLocation();
					// 충돌체크(구충돌)
					// 충돌한 물체를 기억할 배열
					TArray<FOverlapResult> HitObj;;
					FCollisionQueryParams params;
					params.AddIgnoredActor(this);
					// End Overlap 시점에 호출되는 Overlap Multi
					if (GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(500), params))
					{
						// 충돌 배열 순회
						for (int i = 0; i < HitObj.Num(); ++i)
						{
							// 무기 액터 캐스팅
							rifleActor = Cast<ARifleActor>(HitObj[i].GetActor());
							sniperActor = Cast<ASniperActor>(HitObj[i].GetActor());
							pistolActor = Cast<APistolActor>(HitObj[i].GetActor());
							m249Actor = Cast<AM249Actor>(HitObj[i].GetActor());
							MissionChecker = Cast<AMissionChecker>(HitObj[i].GetActor());
							HackingConsole = Cast<AHackingConsole>(HitObj[i].GetActor());
							RifleMagActor = Cast<ARifleMagActor>(HitObj[i].GetActor());
							SniperMagActor = Cast<ASniperMagActor>(HitObj[i].GetActor());
							PistolMagActor = Cast<APistolMagActor>(HitObj[i].GetActor());
							M249MagActor = Cast<AM249MagActor>(HitObj[i].GetActor());
							GoggleActor = Cast<AGoggleActor>(HitObj[i].GetActor());
							MaskActor = Cast<AMaskActor>(HitObj[i].GetActor());
							HelmetActor = Cast<AHelmetActor>(HitObj[i].GetActor());
							HeadsetActor = Cast<AHeadsetActor>(HitObj[i].GetActor());
							ArmorActor = Cast<AArmorActor>(HitObj[i].GetActor());
							MedKitActor = Cast<AMedKitActor>(HitObj[i].GetActor());
							FirstAidKitActor = Cast<AFirstAidKitActor>(HitObj[i].GetActor());
							AdrenalineSyringe = Cast<AAdrenalineSyringe>(HitObj[i].GetActor());
							PoisonOfSpider = Cast<APoisonOfSpider>(HitObj[i].GetActor());
							MilitaryLaptop = Cast<AMilitaryLaptop>(HitObj[i].GetActor());
							MilitaryDevice = Cast<AMilitaryDevice>(HitObj[i].GetActor());
							StageBoard = Cast<AStageBoard>(HitObj[i].GetActor());
							Stash = Cast<AStash>(HitObj[i].GetActor());
							Trader = Cast<ATrader>(HitObj[i].GetActor());
							QuitGameActor = Cast<AQuitGameActor>(HitObj[i].GetActor());
							DeadPlayerContainer = Cast<ADeadPlayerContainer>(HitObj[i].GetActor());

							if (rifleActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								rifleActor->weaponMesh->SetRenderCustomDepth(false);
							}
							else if (sniperActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								sniperActor->weaponMesh->SetRenderCustomDepth(false);
							}
							else if (pistolActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								pistolActor->weaponMesh->SetRenderCustomDepth(false);
							}
							else if (m249Actor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								m249Actor->weaponMesh->SetRenderCustomDepth(false);
							}
							else if (HackingConsole)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								HackingConsole->RootMesh->SetRenderCustomDepth(false);
							}
							else if (MissionChecker)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								MissionChecker->checkerMesh->SetRenderCustomDepth(false);
							}
							else if (RifleMagActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								RifleMagActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (SniperMagActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								SniperMagActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (PistolMagActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								PistolMagActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (M249MagActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								M249MagActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (GoggleActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								GoggleActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (HelmetActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								HelmetActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (HeadsetActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								HeadsetActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (MaskActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								MaskActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (ArmorActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								ArmorActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (MedKitActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								MedKitActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (FirstAidKitActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								FirstAidKitActor->RootMesh->SetRenderCustomDepth(false);
							}
							else if (AdrenalineSyringe)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								AdrenalineSyringe->RootMesh->SetRenderCustomDepth(false);
							}
							else if (PoisonOfSpider)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								PoisonOfSpider->RootMesh->SetRenderCustomDepth(false);
							}
							else if (MilitaryLaptop)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								MilitaryLaptop->RootMesh->SetRenderCustomDepth(false);
							}
							else if (MilitaryDevice)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								MilitaryDevice->RootMesh->SetRenderCustomDepth(false);
							}
							else if (StageBoard)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								StageBoard->boardMesh->SetRenderCustomDepth(false);
							}
							else if (Stash)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								Stash->stashMesh->SetRenderCustomDepth(false);
							}
							else if (Trader)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								Trader->TraderCharacterMesh->SetRenderCustomDepth(false);
							}
							else if (QuitGameActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								QuitGameActor->quitGameMesh->SetRenderCustomDepth(false);
							}
							else if (DeadPlayerContainer)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								DeadPlayerContainer->DeadBodyMesh->SetRenderCustomDepth(false);
							}
						}
					}
				}
			}
		}
		else
		{
		}
	}
}

void APlayerCharacter::DoorInteraction()
{
	DoorInteractionRPCServer();
}

void APlayerCharacter::DoorInteractionRPCServer_Implementation()
{
	DoorInteractionRPCMulticast();
}

bool APlayerCharacter::DoorInteractionRPCServer_Validate()
{
	return true;
}

void APlayerCharacter::DoorInteractionRPCMulticast_Implementation()
{
	DoorInteractionDele.ExecuteIfBound();
}

void APlayerCharacter::SetBossHPWidget(const AEnemy* Enemy) const
{
	if (Enemy && bossHPUI)
	{
		const float BossHP = Enemy->EnemyStat->GetCurrentHp() * 0.0001;
		bossHPUI->progressBar->SetPercent(BossHP);
		const float BossShield = Enemy->EnemyStat->GetCurrentShield() * 0.01;
		bossHPUI->shieldProgressBar->SetPercent(BossShield);
	}
}

void APlayerCharacter::SetDamageWidget(const int Damage, const FVector& SpawnLoc, const bool bIsShieldIconEnable, const FLinearColor DamageTextColor)
{
	if (const ADamageWidgetActor* DamWidget = GetWorld()->SpawnActor<ADamageWidgetActor>(damageWidgetFactory, SpawnLoc + FVector(0, 0, 50), FRotator::ZeroRotator))
	{
		if (UUserWidget* Widui = DamWidget->DamageWidgetComponent->GetUserWidgetObject())
		{
			damageWidgetUI = Cast<UDamageWidget>(Widui);
			if (damageWidgetUI)
			{
				damageWidgetUI->damageText->SetColorAndOpacity(DamageTextColor);
				damageWidgetUI->damage = Damage;
				if (bIsShieldIconEnable)
				{
					damageWidgetUI->ShieldImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}
				else
				{
					damageWidgetUI->ShieldImage->SetVisibility(ESlateVisibility::Hidden);
				}
				if (weaponArray[0] == true)
				{
					damageWidgetUI->rifleBulletImage->SetVisibility(ESlateVisibility::Visible);
					damageWidgetUI->sniperBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->pistolBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->M249BulletImage->SetVisibility(ESlateVisibility::Hidden);
				}
				else if (weaponArray[1] == true)
				{
					damageWidgetUI->rifleBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->sniperBulletImage->SetVisibility(ESlateVisibility::Visible);
					damageWidgetUI->pistolBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->M249BulletImage->SetVisibility(ESlateVisibility::Hidden);
				}
				else if (weaponArray[2] == true)
				{
					damageWidgetUI->rifleBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->sniperBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->pistolBulletImage->SetVisibility(ESlateVisibility::Visible);
					damageWidgetUI->M249BulletImage->SetVisibility(ESlateVisibility::Hidden);
				}
				else if (weaponArray[3] == true)
				{
					damageWidgetUI->rifleBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->sniperBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->pistolBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->M249BulletImage->SetVisibility(ESlateVisibility::Visible);
				}
				damageWidgetUI->PlayAnimation(damageWidgetUI->DamageFloat);
			}
		}
	}
}

void APlayerCharacter::Crouching()
{
}

void APlayerCharacter::ChangeWeapon()
{
	if (bEnding)
	{
		return;
	}
	InteractionProcess();
}

void APlayerCharacter::PickableItemActorInteraction(APickableActor* PickableActor)
{
	PickableItemActorInteractionRPCServer(PickableActor);
}

void APlayerCharacter::PickableItemActorInteractionRPCServer_Implementation(APickableActor* PickableActor)
{
	PickableItemActorInteractionRPCMutlicast(PickableActor);
}

bool APlayerCharacter::PickableItemActorInteractionRPCServer_Validate(APickableActor* PickableActor)
{
	return true;
}

void APlayerCharacter::PickableItemActorInteractionRPCMutlicast_Implementation(APickableActor* PickableActor)
{
	if (PickableActor && !PickableActor->IsAlreadyLooted)
	{
		if (IsLocallyControlled())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickUpSound, GetActorLocation());
		}
		PlayAnimMontage(UpperOnlyMontage, 1, FName("WeaponEquip"));
		FTimerHandle DestroyHandle;
		RifleMagActor = Cast<ARifleMagActor>(PickableActor);
		if (RifleMagActor)
		{
			if (HasAuthority())
			{
				RifleMagActor->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				RifleMagActor->SetLifeSpan(1.f);
				RifleMagActor->SetActorHiddenInGame(true);
				RifleMagActor->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		SniperMagActor = Cast<ASniperMagActor>(PickableActor);
		if (SniperMagActor)
		{
			if (HasAuthority())
			{
				SniperMagActor->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				SniperMagActor->SetLifeSpan(1.f);
				SniperMagActor->SetActorHiddenInGame(true);
				SniperMagActor->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}
			return;
		}
		PistolMagActor = Cast<APistolMagActor>(PickableActor);
		if (PistolMagActor)
		{
			if (HasAuthority())
			{
				PistolMagActor->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				PistolMagActor->SetLifeSpan(1.f);
				PistolMagActor->SetActorHiddenInGame(true);
				PistolMagActor->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		M249MagActor = Cast<AM249MagActor>(PickableActor);
		if (M249MagActor)
		{
			if (HasAuthority())
			{
				M249MagActor->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				M249MagActor->SetLifeSpan(1.f);
				M249MagActor->SetActorHiddenInGame(true);
				M249MagActor->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		GoggleActor = Cast<AGoggleActor>(PickableActor);
		if (GoggleActor)
		{
			if (HasAuthority())
			{
				GoggleActor->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				GoggleActor->SetLifeSpan(1.f);
				GoggleActor->SetActorHiddenInGame(true);
				GoggleActor->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		HelmetActor = Cast<AHelmetActor>(PickableActor);
		if (HelmetActor)
		{
			if (HasAuthority())
			{
				HelmetActor->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				HelmetActor->SetLifeSpan(1.f);
				HelmetActor->SetActorHiddenInGame(true);
				HelmetActor->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		HeadsetActor = Cast<AHeadsetActor>(PickableActor);
		if (HeadsetActor)
		{
			if (HasAuthority())
			{
				HeadsetActor->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				HeadsetActor->SetLifeSpan(1.f);
				HeadsetActor->SetActorHiddenInGame(true);
				HeadsetActor->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		MaskActor = Cast<AMaskActor>(PickableActor);
		if (MaskActor)
		{
			if (HasAuthority())
			{
				MaskActor->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				MaskActor->SetLifeSpan(1.f);
				MaskActor->SetActorHiddenInGame(true);
				MaskActor->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		ArmorActor = Cast<AArmorActor>(PickableActor);
		if (ArmorActor)
		{
			if (HasAuthority())
			{
				ArmorActor->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				ArmorActor->SetLifeSpan(1.f);
				ArmorActor->SetActorHiddenInGame(true);
				ArmorActor->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		MedKitActor = Cast<AMedKitActor>(PickableActor);
		if (MedKitActor)
		{
			if (HasAuthority())
			{
				MedKitActor->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				MedKitActor->SetLifeSpan(1.f);
				MedKitActor->SetActorHiddenInGame(true);
				MedKitActor->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		FirstAidKitActor = Cast<AFirstAidKitActor>(PickableActor);
		if (FirstAidKitActor)
		{
			if (HasAuthority())
			{
				FirstAidKitActor->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				FirstAidKitActor->SetLifeSpan(1.f);
				FirstAidKitActor->SetActorHiddenInGame(true);
				FirstAidKitActor->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		AdrenalineSyringe = Cast<AAdrenalineSyringe>(PickableActor);
		if (AdrenalineSyringe)
		{
			if (HasAuthority())
			{
				AdrenalineSyringe->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				AdrenalineSyringe->SetLifeSpan(1.f);
				AdrenalineSyringe->SetActorHiddenInGame(true);
				AdrenalineSyringe->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		PoisonOfSpider = Cast<APoisonOfSpider>(PickableActor);
		if (PoisonOfSpider)
		{
			if (HasAuthority())
			{
				PoisonOfSpider->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				PoisonOfSpider->SetLifeSpan(1.f);
				PoisonOfSpider->SetActorHiddenInGame(true);
				PoisonOfSpider->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		MilitaryLaptop = Cast<AMilitaryLaptop>(PickableActor);
		if (MilitaryLaptop)
		{
			if (HasAuthority())
			{
				MilitaryLaptop->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				MilitaryLaptop->SetLifeSpan(1.f);
				MilitaryLaptop->SetActorHiddenInGame(true);
				MilitaryLaptop->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		MilitaryDevice = Cast<AMilitaryDevice>(PickableActor);
		if (MilitaryDevice)
		{
			if (HasAuthority())
			{
				MilitaryDevice->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				MilitaryDevice->SetLifeSpan(1.f);
				MilitaryDevice->SetActorHiddenInGame(true);
				MilitaryDevice->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
			}

			return;
		}
		HackingConsole = Cast<AHackingConsole>(PickableActor);
		if (HackingConsole)
		{
			if (HasAuthority())
			{
				HackingConsole->IsAlreadyLooted = true;
				SetActorTickEnabled(false);
				HackingConsole->SetLifeSpan(1.f);
				HackingConsole->SetActorHiddenInGame(true);
				HackingConsole->AddToInventory(this);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();

				ConsoleCount++;
				informationUI->ConsoleCount->SetText(FText::AsNumber(ConsoleCount));
			}
		}
	}
}


void APlayerCharacter::DeadBodyInteraction(ADeadPlayerContainer* DeadPlayerCharacterBox)
{
	DeadBodyInteractionRPCServer(DeadPlayerCharacterBox);
}

void APlayerCharacter::DeadBodyInteractionRPCServer_Implementation(ADeadPlayerContainer* DeadPlayerCharacterBox)
{
	DeadBodyInteractionRPCMutlicast(DeadPlayerCharacterBox);
}

bool APlayerCharacter::DeadBodyInteractionRPCServer_Validate(ADeadPlayerContainer* DeadPlayerCharacterBox)
{
	return true;
}

void APlayerCharacter::DeadBodyInteractionRPCMutlicast_Implementation(ADeadPlayerContainer* DeadPlayerCharacterBox)
{
	if (IsLocallyControlled())
	{
		if (AEclipsePlayerState* EcPlayerState = Cast<AEclipsePlayerState>(GetPlayerState()))
		{
			EcPlayerState->DeadBodyWidgetSettings(DeadPlayerCharacterBox, this);
		}
		UGameplayStatics::PlaySound2D(GetWorld(), tabSound);
		infoWidgetUI->RemoveFromParent();
		PC->SetShowMouseCursor(true);
		DeadBodyWidgetOnViewport();
	}
}

void APlayerCharacter::ChangeWeaponToRifle(ARifleActor* RifleActor)
{
	ChangeWeaponToRifleRPCServer(RifleActor);
}

void APlayerCharacter::ChangeWeaponToRifleRPCServer_Implementation(ARifleActor* RifleActor)
{
	ChangeWeaponToRifleRPCMulticast(RifleActor);
}

bool APlayerCharacter::ChangeWeaponToRifleRPCServer_Validate(ARifleActor* RifleActor)
{
	return true;
}

void APlayerCharacter::ChangeWeaponToRifleRPCMulticast_Implementation(ARifleActor* RifleActor)
{
	if (HasAuthority())
	{
		// 교체 대상 무기 액터 파괴
		RifleActor->Destroy();
	}
	// 무기 교체 Montage 재생
	PlayAnimMontage(UpperOnlyMontage, 1, FName("WeaponEquip"));
	// 액터 스폰 지점 할당
	const FVector spawnPosition = GetMesh()->GetSocketLocation(FName("hand_r"));
	const FRotator spawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// 스나이퍼를 사용중일 때
	if (weaponArray[1] == true)
	{
		// 사용중인 무기 액터 스폰
		GetWorld()->SpawnActor<ASniperActor>(sniperFactory, spawnPosition, spawnRotation, param);
	}
	// 권총을 사용중일 때
	else if (weaponArray[2] == true)
	{
		animInstance->bPistol = false;
		// 사용중인 무기 액터 스폰
		GetWorld()->SpawnActor<APistolActor>(pistolFactory, spawnPosition, spawnRotation, param);
	}
	else if (weaponArray[3] == true)
	{
		// 사용중인 무기 액터 스폰
		GetWorld()->SpawnActor<AM249Actor>(M249Factory, spawnPosition, spawnRotation, param);
	}
	// 현재 활성화된 슬롯이 1번이라면
	if (curWeaponSlotNumber == 1)
	{
		// 장착 무기 이름 배열에 할당
		equippedWeaponStringArray[0] = FString("Rifle");
	}
	// 현재 활성화된 슬롯이 2번이라면
	else if (curWeaponSlotNumber == 2)
	{
		// 장착 무기 이름 배열에 할당
		equippedWeaponStringArray[1] = FString("Rifle");
	}
	// Visibility 설정
	RifleComp->SetVisibility(true);
	SniperComp->SetVisibility(false);
	PistolComp->SetVisibility(false);
	M249Comp->SetVisibility(false);
	// 무기 배열 설정
	weaponArray[0] = true;
	weaponArray[1] = false;
	weaponArray[2] = false;
	weaponArray[3] = false;

	if (IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwapSound);
		// 무기 정보 위젯 제거
		infoWidgetUI->RemoveFromParent();
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSwapSound, GetActorLocation());
	}
}

void APlayerCharacter::ChangeWeaponToSniper(ASniperActor* SniperActor)
{
	ChangeWeaponToSniperRPCServer(SniperActor);
}

void APlayerCharacter::ChangeWeaponToSniperRPCServer_Implementation(ASniperActor* SniperActor)
{
	ChangeWeaponToSniperRPCMulticast(SniperActor);
}

bool APlayerCharacter::ChangeWeaponToSniperRPCServer_Validate(ASniperActor* SniperActor)
{
	return true;
}

void APlayerCharacter::ChangeWeaponToSniperRPCMulticast_Implementation(ASniperActor* SniperActor)
{
	if (HasAuthority())
	{
		SniperActor->Destroy();
	}
	PlayAnimMontage(UpperOnlyMontage, 1, FName("WeaponEquip"));
	const FVector spawnPosition = GetMesh()->GetSocketLocation(FName("hand_r"));
	const FRotator spawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (weaponArray[0] == true)
	{
		GetWorld()->SpawnActor<ARifleActor>(rifleFactory, spawnPosition, spawnRotation, param);
	}
	else if (weaponArray[2] == true)
	{
		animInstance->bPistol = false;
		GetWorld()->SpawnActor<APistolActor>(pistolFactory, spawnPosition, spawnRotation, param);
	}
	else if (weaponArray[3] == true)
	{
		GetWorld()->SpawnActor<AM249Actor>(M249Factory, spawnPosition, spawnRotation, param);
	}
	if (curWeaponSlotNumber == 1)
	{
		equippedWeaponStringArray[0] = FString("Sniper");
	}
	else if (curWeaponSlotNumber == 2)
	{
		equippedWeaponStringArray[1] = FString("Sniper");
	}
	RifleComp->SetVisibility(false);
	SniperComp->SetVisibility(true);
	PistolComp->SetVisibility(false);
	M249Comp->SetVisibility(false);

	weaponArray[0] = false;
	weaponArray[1] = true;
	weaponArray[2] = false;
	weaponArray[3] = false;

	if (IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwapSound);
		// 무기 정보 위젯 제거
		infoWidgetUI->RemoveFromParent();
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSwapSound, GetActorLocation());
	}
}

void APlayerCharacter::ChangeWeaponToPistol(APistolActor* PistolActor)
{
	ChangeWeaponToPistolRPCServer(PistolActor);
}

void APlayerCharacter::ChangeWeaponToPistolRPCServer_Implementation(APistolActor* PistolActor)
{
	ChangeWeaponToPistolRPCMulticast(PistolActor);
}

bool APlayerCharacter::ChangeWeaponToPistolRPCServer_Validate(APistolActor* PistolActor)
{
	return true;
}

void APlayerCharacter::ChangeWeaponToPistolRPCMulticast_Implementation(APistolActor* PistolActor)
{
	if (HasAuthority())
	{
		PistolActor->Destroy();
	}
	PlayAnimMontage(UpperOnlyMontage, 1, FName("PistolEquip"));
	animInstance->bPistol = true;
	const FVector spawnPosition = GetMesh()->GetSocketLocation(FName("hand_r"));
	const FRotator spawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (weaponArray[0] == true)
	{
		GetWorld()->SpawnActor<ARifleActor>(rifleFactory, spawnPosition, spawnRotation, param);
	}
	else if (weaponArray[1] == true)
	{
		GetWorld()->SpawnActor<ASniperActor>(sniperFactory, spawnPosition, spawnRotation, param);
	}
	else if (weaponArray[3] == true)
	{
		GetWorld()->SpawnActor<AM249Actor>(M249Factory, spawnPosition, spawnRotation, param);
	}
	if (curWeaponSlotNumber == 1)
	{
		equippedWeaponStringArray[0] = FString("Pistol");
	}
	else if (curWeaponSlotNumber == 2)
	{
		equippedWeaponStringArray[1] = FString("Pistol");
	}
	RifleComp->SetVisibility(false);
	SniperComp->SetVisibility(false);
	PistolComp->SetVisibility(true);
	M249Comp->SetVisibility(false);

	weaponArray[0] = false;
	weaponArray[1] = false;
	weaponArray[2] = true;
	weaponArray[3] = false;

	if (IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwapSound);
		// 무기 정보 위젯 제거
		infoWidgetUI->RemoveFromParent();
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSwapSound, GetActorLocation());
	}
}

void APlayerCharacter::ChangeWeaponToM249(AM249Actor* M249Actor)
{
	ChangeWeaponToM249RPCServer(M249Actor);
}

void APlayerCharacter::ChangeWeaponToM249RPCServer_Implementation(AM249Actor* M249Actor)
{
	ChangeWeaponToM249RPCMulticast(M249Actor);
}

bool APlayerCharacter::ChangeWeaponToM249RPCServer_Validate(AM249Actor* M249Actor)
{
	return true;
}

void APlayerCharacter::ChangeWeaponToM249RPCMulticast_Implementation(AM249Actor* M249Actor)
{
	if (HasAuthority())
	{
		M249Actor->Destroy();
	}
	PlayAnimMontage(UpperOnlyMontage, 1, FName("WeaponEquip"));
	const FVector spawnPosition = GetMesh()->GetSocketLocation(FName("hand_r"));
	const FRotator spawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (weaponArray[0] == true)
	{
		GetWorld()->SpawnActor<ARifleActor>(rifleFactory, spawnPosition, spawnRotation);
	}
	else if (weaponArray[1] == true)
	{
		GetWorld()->SpawnActor<ASniperActor>(sniperFactory, spawnPosition, spawnRotation);
	}
	else if (weaponArray[2] == true)
	{
		animInstance->bPistol = false;
		GetWorld()->SpawnActor<APistolActor>(pistolFactory, spawnPosition, spawnRotation);
	}
	if (curWeaponSlotNumber == 1)
	{
		equippedWeaponStringArray[0] = FString("M249");
	}
	else if (curWeaponSlotNumber == 2)
	{
		equippedWeaponStringArray[1] = FString("M249");
	}
	RifleComp->SetVisibility(false);
	SniperComp->SetVisibility(false);
	PistolComp->SetVisibility(false);
	M249Comp->SetVisibility(true);

	weaponArray[0] = false;
	weaponArray[1] = false;
	weaponArray[2] = false;
	weaponArray[3] = true;

	if (IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwapSound);
		// 무기 정보 위젯 제거
		infoWidgetUI->RemoveFromParent();
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSwapSound, GetActorLocation());
	}
}


void APlayerCharacter::InteractionProcess()
{
	FHitResult actorHitResult;
	FVector StartLoc = FollowCamera->GetComponentLocation();
	FVector EndLoc = StartLoc + FollowCamera->GetForwardVector() * 500.0f;
	// 무기 액터 탐지 라인 트레이스
	if (GetWorld()->LineTraceSingleByChannel(actorHitResult, StartLoc, EndLoc, ECC_Visibility))
	{
		// 무기 액터 캐스팅
		rifleActor = Cast<ARifleActor>(actorHitResult.GetActor());
		sniperActor = Cast<ASniperActor>(actorHitResult.GetActor());
		pistolActor = Cast<APistolActor>(actorHitResult.GetActor());
		m249Actor = Cast<AM249Actor>(actorHitResult.GetActor());
		PickableItemActor = Cast<APickableActor>(actorHitResult.GetActor());
		StageBoard = Cast<AStageBoard>(actorHitResult.GetActor());
		Stash = Cast<AStash>(actorHitResult.GetActor());
		Trader = Cast<ATrader>(actorHitResult.GetActor());
		QuitGameActor = Cast<AQuitGameActor>(actorHitResult.GetActor());
		DeadPlayerContainer = Cast<ADeadPlayerContainer>(actorHitResult.GetActor());

		// 라이플로 교체
		if (rifleActor)
		{
			// 라이플을 사용하지 않을 때만 교체
			if (weaponArray[0] == false)
			{
				// 키다운 시간 동안 Radial Slider 게이지 상승
				infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
				// 게이지가 모두 채워졌을 때
				if (infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
				{
					infoWidgetUI->weaponHoldPercent = 0;
					ChangeWeaponToRifle(rifleActor);
				}
			}
		}
		// 스나이퍼로 교체
		else if (sniperActor)
		{
			// 스나이퍼를 사용하지 않을 때만 교체
			if (weaponArray[1] == false)
			{
				// 키다운 시간 동안 Radial Slider 게이지 상승
				infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
				if (infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
				{
					infoWidgetUI->weaponHoldPercent = 0;
					ChangeWeaponToSniper(sniperActor);
				}
			}
		}
		// 권총으로 교체
		else if (pistolActor)
		{
			// 권총을 사용하지 않을 때만 교체
			if (weaponArray[2] == false)
			{
				// 키다운 시간 동안 Radial Slider 게이지 상승
				infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
				if (infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
				{
					infoWidgetUI->weaponHoldPercent = 0;
					ChangeWeaponToPistol(pistolActor);
				}
			}
		}
		// M249로 교체
		else if (m249Actor)
		{
			// M249을 사용하지 않을 때만 교체
			if (weaponArray[3] == false)
			{
				// 키다운 시간 동안 Radial Slider 게이지 상승
				infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
				if (infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
				{
					infoWidgetUI->weaponHoldPercent = 0;
					ChangeWeaponToM249(m249Actor);
				}
			}
		}
		else if (PickableItemActor)
		{
			// 키다운 시간 동안 Radial Slider 게이지 상승
			infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
			if (infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
			{
				infoWidgetUI->weaponHoldPercent = 0;
				PickableItemActorInteraction(PickableItemActor);
			}
		}
		else if (MissionChecker)
		{
			// 키다운 시간 동안 Radial Slider 게이지 상승
			infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
			if (infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
			{
				if (GuardianCount >= 7 && ConsoleCount >= 5 && BossCount >= 1)
				{
					infoWidgetUI->weaponHoldPercent = 0;
					bEnding = true;
					APlayerCameraManager* playerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
					playerCam->StartCameraFade(0, 1, 7.0, FLinearColor::Black, false, true);
					StopAnimMontage();
					GetCharacterMovement()->StopActiveMovement();
					GetCharacterMovement()->DisableMovement();
					FTransform spawnTrans = this->GetTransform();
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), recallParticle, spawnTrans);
					PlayAnimMontage(FullBodyMontage, 1, FName("LevelEnd"));
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), PortalSound, GetActorLocation());
					bUseControllerRotationYaw = false;
					infoWidgetUI->RemoveFromParent();
					informationUI->RemoveFromParent();
					crosshairUI->RemoveFromParent();
					FTimerHandle endHandle;
					GetWorldTimerManager().SetTimer(endHandle, FTimerDelegate::CreateLambda([this]()-> void
					{
						// if (AEclipsePlayerState* CachingPlayerState = Cast<AEclipsePlayerState>(GetPlayerState()))
						// {
						// 	CachingPlayerState->InventoryCaching(this);
						// }
						UGameplayStatics::OpenLevel(GetWorld(), FName("Safe_House"));
					}), 9.f, false);
				}
				else
				{
					infoWidgetUI->PlayAnimation(infoWidgetUI->LackMission);
					infoWidgetUI->weaponHoldPercent = 0;
				}
			}
		}

		else if (StageBoard)
		{
			// 키다운 시간 동안 Radial Slider 게이지 상승
			infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
			if (levelSelectionUI && infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
			{
				infoWidgetUI->weaponHoldPercent = 0;
				UGameplayStatics::PlaySound2D(GetWorld(), levelSelectionSound);
				infoWidgetUI->RemoveFromParent();
				UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, levelSelectionUI);
				PC->SetShowMouseCursor(true);
				levelSelectionUI->AddToViewport();
			}
		}
		else if (Stash)
		{
			// 키다운 시간 동안 Radial Slider 게이지 상승
			infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
			if (infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
			{
				infoWidgetUI->weaponHoldPercent = 0;
				if (bStashWidgetOn == false && PC)
				{
					bStashWidgetOn = true;
					UGameplayStatics::PlaySound2D(GetWorld(), tabSound);
					infoWidgetUI->RemoveFromParent();
					PC->SetShowMouseCursor(true);
					StashWidgetOnViewport();
				}
			}
		}
		else if (Trader)
		{
			// 키다운 시간 동안 Radial Slider 게이지 상승
			infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
			if (TradeWidgetUI && infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
			{
				infoWidgetUI->weaponHoldPercent = 0;
				gi->IsWidgetOn = true;
				UGameplayStatics::PlaySound2D(GetWorld(), quitGameSound);
				infoWidgetUI->RemoveFromParent();
				UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, TradeWidgetUI);
				PC->SetShowMouseCursor(true);
				TradeWidgetUI->AddToViewport();
			}
		}
		else if (QuitGameActor)
		{
			// 키다운 시간 동안 Radial Slider 게이지 상승
			infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
			if (quitWidgetUI && infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
			{
				infoWidgetUI->weaponHoldPercent = 0;
				if (!quitWidgetUI->IsInViewport() && PC)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), quitGameSound);
					infoWidgetUI->RemoveFromParent();
					UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, quitWidgetUI);
					PC->SetShowMouseCursor(true);
					quitWidgetUI->AddToViewport();
				}
			}
		}
		else if (DeadPlayerContainer)
		{
			// 키다운 시간 동안 Radial Slider 게이지 상승
			infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
			if (quitWidgetUI && infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
			{
				infoWidgetUI->weaponHoldPercent = 0;
				if (bDeadBodyWidgetOn == false)
				{
					bDeadBodyWidgetOn = true;
					DeadBodyInteraction(DeadPlayerContainer);
				}
			}
		}
	}
}


void APlayerCharacter::Reload()
{
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != FString("Safe_House"))
	{
		ServerRPCReload();
	}
}

void APlayerCharacter::ServerRPCReload_Implementation()
{
	MulticastRPCReload();
}

void APlayerCharacter::MulticastRPCReload_Implementation()
{
	if (const bool IsMontagePlaying = animInstance->IsAnyMontagePlaying(); !IsMontagePlaying)
	{
		if (weaponArray[0] == true && curRifleAmmo < 40 && maxRifleAmmo > 0)
		{
			if (IsLocallyControlled())
			{
				SetFirstPersonModeRifle(false);
				crosshairUI->PlayAnimation(crosshairUI->ReloadAnimation);
				UGameplayStatics::PlaySound2D(GetWorld(), RifleReloadSound);
			}
			else
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), RifleReloadSound, GetActorLocation());
			}

			PlayAnimMontage(UpperOnlyMontage, 1, FName("Reload"));
		}
		else if (weaponArray[1] == true && curSniperAmmo < 5 && maxSniperAmmo > 0)
		{
			if (IsLocallyControlled())
			{
				crosshairUI->PlayAnimation(crosshairUI->ReloadAnimation);
				UGameplayStatics::PlaySound2D(GetWorld(), SniperReloadSound);
			}
			else
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), SniperReloadSound, GetActorLocation());
			}
			PlayAnimMontage(UpperOnlyMontage, 1, FName("Reload"));
		}
		else if (weaponArray[2] == true && curPistolAmmo < 8 && maxPistolAmmo > 0)
		{
			if (IsLocallyControlled())
			{
				SetFirstPersonModePistol(false);
				crosshairUI->PlayAnimation(crosshairUI->ReloadAnimation);
				UGameplayStatics::PlaySound2D(GetWorld(), PistolReloadSound);
			}
			else
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), PistolReloadSound, GetActorLocation());
			}
			PlayAnimMontage(UpperOnlyMontage, 1, FName("PistolReload"));
		}
		else if (weaponArray[3] == true && curM249Ammo < 100 && maxM249Ammo > 0)
		{
			if (IsLocallyControlled())
			{
				crosshairUI->PlayAnimation(crosshairUI->ReloadAnimation);
				UGameplayStatics::PlaySound2D(GetWorld(), M249ReloadSound);
			}
			else
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), M249ReloadSound, GetActorLocation());
			}
			PlayAnimMontage(UpperOnlyMontage, 1, FName("M249Reload"));
		}
	}
}


bool APlayerCharacter::ServerRPCReload_Validate()
{
	return true;
}

void APlayerCharacter::MoveToIsolatedShip()
{
	bEnding = true;
	gi->IsWidgetOn = false;
	APlayerCameraManager* PlayerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	PlayerCam->StartCameraFade(0, 1, 7.0, FLinearColor::Black, false, true);
	StopAnimMontage();
	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->DisableMovement();
	const FTransform SpawnTrans = this->GetTransform();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), recallParticle, SpawnTrans);
	PlayAnimMontage(FullBodyMontage, 1, FName("LevelEnd"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PortalSound, GetActorLocation());
	bUseControllerRotationYaw = false;
	infoWidgetUI->RemoveFromParent();
	informationUI->RemoveFromParent();
	crosshairUI->RemoveFromParent();
	FTimerHandle EndHandle;
	GetWorldTimerManager().SetTimer(EndHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		if (AEclipsePlayerState* CachingPlayerState = Cast<AEclipsePlayerState>(GetPlayerState())) CachingPlayerState->CacheInventoryDataToGameInstance();
		UGameplayStatics::OpenLevel(GetWorld(), FName("Map_BigStarStation"));
	}), 9.f, false);
}

void APlayerCharacter::MoveToHideout()
{
	APlayerCameraManager* PlayerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	PlayerCam->StartCameraFade(0, 1, 2.0, FLinearColor::Black, false, true);
	FTimerHandle EndHandle;
	GetWorldTimerManager().SetTimer(EndHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		if (AEclipsePlayerState* CachingPlayerState = Cast<AEclipsePlayerState>(GetPlayerState())) CachingPlayerState->CacheInventoryDataToGameInstance();
		UGameplayStatics::OpenLevel(GetWorld(), FName("Safe_House"));
	}), 2.f, false);
}

void APlayerCharacter::MoveToBlockedIntersection()
{
	bEnding = true;
	gi->IsWidgetOn = false;
	APlayerCameraManager* PlayerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	PlayerCam->StartCameraFade(0, 1, 7.0, FLinearColor::Black, false, true);
	StopAnimMontage();
	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->DisableMovement();
	const FTransform SpawnTrans = this->GetTransform();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), recallParticle, SpawnTrans);
	PlayAnimMontage(FullBodyMontage, 1, FName("LevelEnd"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PortalSound, GetActorLocation());
	bUseControllerRotationYaw = false;
	infoWidgetUI->RemoveFromParent();
	informationUI->RemoveFromParent();
	crosshairUI->RemoveFromParent();
	FTimerHandle EndHandle;
	GetWorldTimerManager().SetTimer(EndHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		if (AEclipsePlayerState* CachingPlayerState = Cast<AEclipsePlayerState>(GetPlayerState())) CachingPlayerState->CacheInventoryDataToGameInstance();
		UGameplayStatics::OpenLevel(GetWorld(), FName("192.168.0.3"));
	}), 9.f, false);
}


void APlayerCharacter::SetZoomValue(const float Value)
{
	if (weaponArray[1] == true && !SniperZoomBool && !SniperZoomOutBool)
	{
		// 타임라인 Float Curve 에 따른 Lerp
		const double Lerp = UKismetMathLibrary::Lerp(90, 40, Value);
		// 해당 Lerp값 Arm Length에 적용
		FollowCamera->SetFieldOfView(Lerp);
	}
	else if (weaponArray[1] == false)
	{
		// 타임라인 Float Curve 에 따른 Lerp
		const double Lerp = UKismetMathLibrary::Lerp(200, 100, Value);
		// 해당 Lerp값 Arm Length에 적용
		CameraBoom->TargetArmLength = Lerp;
	}
	else if (SniperZoomBool)
	{
		const double Lerp = UKismetMathLibrary::Lerp(40, 20, Value);
		FollowCamera->SetFieldOfView(Lerp);
		GetWorldTimerManager().SetTimer(SniperZoomHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			SniperZoomBool = false;
		}), 1.0f, false);
	}
	else if (SniperZoomOutBool)
	{
		const float Lerp = FollowCamera->FieldOfView = UKismetMathLibrary::Lerp(20, 40, Value);
		FollowCamera->SetFieldOfView(Lerp);
		GetWorldTimerManager().SetTimer(SniperZoomOutHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			SniperZoomOutBool = false;
		}), 1.0f, false);
	}
}

void APlayerCharacter::CachingValues() const
{
	if (gi)
	{
		gi->ConsoleCount = ConsoleCount;

		gi->GuardianCount = GuardianCount;

		gi->BossCount = BossCount;

		gi->curRifleAmmo = curRifleAmmo;

		gi->curSniperAmmo = curSniperAmmo;

		gi->curPistolAmmo = curPistolAmmo;

		gi->curM249Ammo = curM249Ammo;

		gi->maxRifleAmmo = maxRifleAmmo;

		gi->maxSniperAmmo = maxSniperAmmo;

		gi->maxPistolAmmo = maxPistolAmmo;

		gi->maxM249Ammo = maxM249Ammo;
	}
}

void APlayerCharacter::ApplyCachingValues()
{
	if (gi)
	{
		ConsoleCount = gi->ConsoleCount;

		GuardianCount = gi->GuardianCount;

		BossCount = gi->BossCount;

		curRifleAmmo = gi->curRifleAmmo;

		curSniperAmmo = gi->curSniperAmmo;

		curPistolAmmo = gi->curPistolAmmo;

		curM249Ammo = gi->curM249Ammo;

		maxRifleAmmo = gi->maxRifleAmmo;

		maxSniperAmmo = gi->maxSniperAmmo;

		maxPistolAmmo = gi->maxPistolAmmo;

		maxM249Ammo = gi->maxM249Ammo;
	}
}

void APlayerCharacter::Damaged(const int Damage, AActor* DamageCauser)
{
	DamagedRPCServer(Damage, DamageCauser);
	Stat->ApplyDamage(Damage, DamageCauser);
}

void APlayerCharacter::DamagedRPCServer_Implementation(int Damage, AActor* DamageCauser)
{
	DamagedRPCMulticast(Damage, DamageCauser);
}

bool APlayerCharacter::DamagedRPCServer_Validate(int Damage, AActor* DamageCauser)
{
	return true;
}

void APlayerCharacter::DamagedRPCMulticast_Implementation(int Damage, AActor* DamageCauser)
{
	if (HasAuthority())
	{
		//Stat->ApplyDamage(damage, DamageCauser);
	}
	if (IsLocallyControlled())
	{
		crosshairUI->PlayAnimation(crosshairUI->DamagedAnimation);
		UGameplayStatics::PlaySound2D(GetWorld(), DamagedSound);
		PC->PlayerCameraManager->StartCameraShake(PlayerDamagedShake);
	}
	UpdateTabWidget();
	StopAnimMontage();
	PlayAnimMontage(FullBodyMontage, 1, FName("Damaged"));
	FTimerHandle overlayMatHandle;
	GetMesh()->SetOverlayMaterial(overlayMatRed);
	GetWorldTimerManager().ClearTimer(overlayMatHandle);
	GetWorldTimerManager().SetTimer(overlayMatHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		GetMesh()->SetOverlayMaterial(nullptr);
	}), 0.3f, false);
}

void APlayerCharacter::OnRep_WeaponArrayChanged() const
{
	WeaponChangeDele.Broadcast();
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, CanShoot);
	DOREPLIFETIME(APlayerCharacter, curRifleAmmo);
	DOREPLIFETIME(APlayerCharacter, curSniperAmmo);
	DOREPLIFETIME(APlayerCharacter, curPistolAmmo);
	DOREPLIFETIME(APlayerCharacter, curM249Ammo);
	DOREPLIFETIME(APlayerCharacter, maxRifleAmmo);
	DOREPLIFETIME(APlayerCharacter, maxSniperAmmo);
	DOREPLIFETIME(APlayerCharacter, maxPistolAmmo);
	DOREPLIFETIME(APlayerCharacter, maxM249Ammo);
	DOREPLIFETIME(APlayerCharacter, IsPlayerDead);
	DOREPLIFETIME(APlayerCharacter, IsPlayerDeadImmediately);
	DOREPLIFETIME(APlayerCharacter, weaponArray);
	DOREPLIFETIME(APlayerCharacter, IsEquipArmor);
	DOREPLIFETIME(APlayerCharacter, IsEquipHelmet);
	DOREPLIFETIME(APlayerCharacter, IsEquipGoggle);
	DOREPLIFETIME(APlayerCharacter, IsEquipMask);
	DOREPLIFETIME(APlayerCharacter, IsEquipHeadset);
	DOREPLIFETIME(APlayerCharacter, DamageAmount);
}

void APlayerCharacter::OnRep_IsEquipArmor() const
{
	if (IsEquipArmor)
	{
		ArmorSlot->SetVisibility(true);
	}
	else
	{
		ArmorSlot->SetVisibility(false);
	}
}

void APlayerCharacter::OnRep_IsEquipHelmet() const
{
	if (IsEquipHelmet)
	{
		HelmetSlot->SetVisibility(true);
	}
	else
	{
		HelmetSlot->SetVisibility(false);
	}
}

void APlayerCharacter::OnRep_IsEquipGoggle() const
{
	if (IsEquipGoggle)
	{
		GoggleSlot->SetVisibility(true);
	}
	else
	{
		GoggleSlot->SetVisibility(false);
	}
}

void APlayerCharacter::OnRep_IsEquipMask() const
{
	if (IsEquipMask)
	{
		MaskSlot->SetVisibility(true);
	}
	else
	{
		MaskSlot->SetVisibility(false);
	}
}

void APlayerCharacter::OnRep_IsEquipHeadset() const
{
	if (IsEquipHeadset)
	{
		HeadSetSlot->SetVisibility(true);
	}
	else
	{
		HeadSetSlot->SetVisibility(false);
	}
}

void APlayerCharacter::Fire()
{
	// 사격 가능 상태가 아니거나, 뛰고 있거나, 위젯이 켜져 있거나, 엔딩 연출 중이라면 리턴
	if (!CanShoot || isRunning || gi->IsWidgetOn || bEnding || IsPlayerDeadImmediately || UGameplayStatics::GetCurrentLevelName(GetWorld()) == FString("Safe_House"))
	{
		return;
	}
	if (!isZooming && weaponArray[1] == false && weaponArray[2] == false)
	{
		GetWorldTimerManager().ClearTimer(ZoomFireHandle);
		Zoom(false);
	}
	FireLocal();
	ServerRPCFire();
	CanShoot = false;
	GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		CanShoot = true;
	}), 1 / SetFireInterval(), false);
}

void APlayerCharacter::FireLocal()
{
	// Rifle
	if (weaponArray[0] == true)
	{
		if (curRifleAmmo > 0)
		{
			ProcessRifleFireLocal();
		}
	}
	// Sniper
	else if (weaponArray[1] == true)
	{
		if (curSniperAmmo > 0)
		{
			ProcessSniperFireLocal();
		}
	}
	// Pistol
	else if (weaponArray[2] == true)
	{
		if (curPistolAmmo > 0)
		{
			ProcessPistolFireLocal();
		}
	}
	// M249
	else if (weaponArray[3] == true)
	{
		if (curM249Ammo > 0)
		{
			ProcessM249FireLocal();
		}
	}
}

bool APlayerCharacter::ServerRPCFire_Validate()
{
	return true;
}

void APlayerCharacter::ServerRPCFire_Implementation()
{
	MulticastRPCFire();
}

void APlayerCharacter::MulticastRPCFire_Implementation()
{
	// Rifle
	if (weaponArray[0] == true)
	{
		if (curRifleAmmo > 0)
		{
			ProcessRifleFireAnim();
			
			if (!IsLocallyControlled())
			{
				ProcessRifleFireSimulatedProxy();
			}			

			// 서버 로직 (핵심 프로세스 처리)
			if (HasAuthority())
			{
				ProcessRifleFire();
			}
		}
		// No Ammo
		else
		{
			AmmoDepleted();
		}
		return;
	}
	// Sniper
	if (weaponArray[1] == true)
	{
		if (curSniperAmmo > 0)
		{
			ProcessSniperFireAnim();

			if (!IsLocallyControlled())
			{
				ProcessSniperFireSimulatedProxy();
			}
			
			// 서버 로직 (핵심 프로세스 처리)
			if (HasAuthority())
			{
				ProcessSniperFire();
			}
		}
		// No Ammo
		else
		{
			AmmoDepleted();
		}
		return;
	}
	// Pistol
	if (weaponArray[2] == true)
	{
		if (curPistolAmmo > 0)
		{
			ProcessPistolFireAnim();

			if (!IsLocallyControlled())
			{
				ProcessPistolFireSimulatedProxy();
			}			
			
			// 서버 로직 (핵심 프로세스 처리)
			if (HasAuthority())
			{
				ProcessPistolFire();
			}
		}
		else
		{
			AmmoDepleted();
		}
		return;
	}
	// M249
	if (weaponArray[3] == true)
	{
		if (curM249Ammo > 0)
		{
			ProcessM249FireAnim();
			
			if (!IsLocallyControlled())
			{
				ProcessM249FireSimulatedProxy();
			}
			
			// 서버 로직 (핵심 프로세스 처리)
			if (HasAuthority())
			{
				ProcessM249Fire();
			}
		}
		else
		{
			AmmoDepleted();
		}
	}
}

void APlayerCharacter::AmmoDepleted()
{
	if (EmptySoundBoolean == false)
	{
		EmptySoundBoolean = true;
		if (IsLocallyControlled())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), BulletEmptySound);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletEmptySound, GetActorLocation());
		}
	}
}

void APlayerCharacter::ExtractionSuccess() const
{
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PlayerSpawnEmitter, GetActorLocation());
	UGameplayStatics::PlaySound2D(GetWorld(), ExtractionSound);
	APlayerCameraManager* PlayerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	// 카메라 페이드 연출
	PlayerCam->StartCameraFade(0, 1, 1.5f, FLinearColor::Black, false, true);
	FTimerHandle ExtractionHandle;
	GetWorld()->GetTimerManager().SetTimer(ExtractionHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("Safe_House"), true);
	}), 1.5f, false);
}

void APlayerCharacter::SetFirstPersonModeRifle(const bool IsFirstPerson)
{
	if (IsFirstPerson)
	{
		crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
		GetMesh()->SetVisibility(false);
		RifleComp->SetVisibility(false);
		FirstPersonRifleComp->SetVisibility(true);
		FirstPersonCharacterMesh->SetVisibility(true);
		FollowCamera->SetActive(false);
		FirstPersonCamera->SetActive(true);
		if (const auto FirstAnimInstance = Cast<UFirstPersonPlayerAnim>(FirstPersonCharacterMesh->GetAnimInstance()))
		{
			FirstAnimInstance->bPistol = false;
			FirstAnimInstance->Montage_Play(FirstPersonRifeZoomMontage, 1);
		}
	}
	else
	{
		crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Visible);
		GetMesh()->SetVisibility(true);
		RifleComp->SetVisibility(true);
		FirstPersonRifleComp->SetVisibility(false);
		FirstPersonCharacterMesh->SetVisibility(false);
		FollowCamera->SetActive(true);
		FirstPersonCamera->SetActive(false);
	}
}

void APlayerCharacter::SetFirstPersonModePistol(const bool IsFirstPerson)
{
	if (IsFirstPerson)
	{
		crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
		GetMesh()->SetVisibility(false);
		PistolComp->SetVisibility(false);
		FirstPersonPistolComp->SetVisibility(true);
		FirstPersonCharacterMesh->SetVisibility(true);
		FollowCamera->SetActive(false);
		FirstPersonCamera->SetActive(true);
		if (const auto FirstAnimInstance = Cast<UFirstPersonPlayerAnim>(FirstPersonCharacterMesh->GetAnimInstance()))
		{
			FirstAnimInstance->bPistol = true;
			FirstAnimInstance->Montage_Play(FirstPersonPistolZoomMontage, 1);
		}
	}
	else
	{
		crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Visible);
		GetMesh()->SetVisibility(true);
		PistolComp->SetVisibility(true);
		FirstPersonPistolComp->SetVisibility(false);
		FirstPersonCharacterMesh->SetVisibility(false);
		FollowCamera->SetActive(true);
		FirstPersonCamera->SetActive(false);
	}
}

void APlayerCharacter::EquipArmorInventorySlot(const bool IsEquipping)
{
	if (IsEquipping)
	{
		IsEquipArmor = true;
		OnRep_IsEquipArmor();
	}
	else
	{
		IsEquipArmor = false;
		OnRep_IsEquipArmor();
	}
}

void APlayerCharacter::EquipHelmetInventorySlot(const bool IsEquipping)
{
	if (IsEquipping)
	{
		IsEquipHelmet = true;
		OnRep_IsEquipHelmet();
	}
	else
	{
		IsEquipHelmet = false;
		OnRep_IsEquipHelmet();
	}
}

void APlayerCharacter::EquipGoggleInventorySlot(const bool IsEquipping)
{
	if (IsEquipping)
	{
		IsEquipGoggle = true;
		OnRep_IsEquipGoggle();
	}
	else
	{
		IsEquipGoggle = false;
		OnRep_IsEquipGoggle();
	}
}

void APlayerCharacter::EquipHeadsetInventorySlot(const bool IsEquipping)
{
	if (IsEquipping)
	{
		IsEquipHeadset = true;
		OnRep_IsEquipHeadset();
	}
	else
	{
		IsEquipHeadset = false;
		OnRep_IsEquipHeadset();
	}
}

void APlayerCharacter::EquipMaskInventorySlot(const bool IsEquipping)
{
	if (IsEquipping)
	{
		IsEquipMask = true;
		OnRep_IsEquipMask();
	}
	else
	{
		IsEquipMask = false;
		OnRep_IsEquipMask();
	}
}

void APlayerCharacter::ProcessRifleFireAnim()
{
	StopAnimMontage();
	PlayAnimMontage(RifleFireMontage, 1, FName("Fire"));
}

void APlayerCharacter::ProcessRifleFireLocal()
{
	UGameplayStatics::PlaySound2D(GetWorld(), RifleFireSound);
	// 사격 카메라 셰이크 실행
	PC->PlayerCameraManager->StartCameraShake(rifleFireShake);
	if (FirstPersonCharacterMesh->IsVisible())
	{
		const FVector particleLoc = FirstPersonRifleComp->GetSocketLocation(FName("RifleFirePosition"));
		const UE::Math::TRotator<double> particleRot = FirstPersonRifleComp->GetSocketRotation(FName("RifleFirePosition"));
		const FTransform particleTrans = UKismetMathLibrary::MakeTransform(particleLoc, particleRot, FVector(0.4));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FirstPersonRifleFireParticle, particleTrans);
	}
	else
	{
		const FVector particleLoc = RifleComp->GetSocketLocation(FName("RifleFirePosition"));
		const UE::Math::TRotator<double> particleRot = RifleComp->GetSocketRotation(FName("RifleFirePosition"));
		const FTransform particleTrans = UKismetMathLibrary::MakeTransform(particleLoc, particleRot, FVector(0.4));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RifleFireParticle, particleTrans);
	}
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), RifleFireSound, GetActorLocation());

	const double RandF = UKismetMathLibrary::RandomFloatInRange(-0.3, -0.5);
	const double RandF2 = UKismetMathLibrary::RandomFloatInRange(-0.3, 0.3);
	AddControllerPitchInput(RandF);
	AddControllerYawInput(RandF2);
}

void APlayerCharacter::ProcessRifleFireSimulatedProxy() const
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), RifleFireSound, GetActorLocation());
	const FVector ParticleLoc = RifleComp->GetSocketLocation(FName("RifleFirePosition"));
	const UE::Math::TRotator<double> particleRot = RifleComp->GetSocketRotation(FName("RifleFirePosition"));
	const FTransform particleTrans = UKismetMathLibrary::MakeTransform(ParticleLoc, particleRot, FVector(0.4));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RifleFireParticle, particleTrans);
}

void APlayerCharacter::FireRelease()
{
	if (!IsZoomKeyPressed && weaponArray[1] == false && weaponArray[2] == false && UGameplayStatics::GetCurrentLevelName(GetWorld()) != FString("Safe_House"))
	{
		GetWorld()->GetTimerManager().SetTimer(ZoomFireHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			ZoomRelease(false);
		}), 0.5f, false);
	}
	EmptySoundBoolean = false;
}

void APlayerCharacter::ProcessRifleFire()
{
	if (curRifleAmmo > 0)
	{
		// Clamp를 통한 탄약 수 차감
		curRifleAmmo = FMath::Clamp(curRifleAmmo - 1, 0, 40);
		if (FirstPersonRifleComp->IsVisible())
		{
			RifleLineTraceStart = FirstPersonCamera->GetComponentLocation();
			RifleLineTraceEnd = RifleLineTraceStart + FirstPersonCamera->GetForwardVector() * 10000.0f;
		}
		else
		{
			RifleLineTraceStart = FollowCamera->GetComponentLocation();
			RifleLineTraceEnd = RifleLineTraceStart + FollowCamera->GetForwardVector() * 10000.0f;
		}
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		// Perform Linetrace
		if (FHitResult RifleHitResult; GetWorld()->LineTraceSingleByChannel(RifleHitResult, RifleLineTraceStart, RifleLineTraceEnd, ECC_Visibility, Params))
		{
			// Player Character Casting
			// 플레이어 적중
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(RifleHitResult.GetActor()))
			{
				if (RifleHitResult.BoneName == FName("head"))
				{
					OnPlayerHit(RifleHitResult, Player, true);
				}
				else
				{
					OnPlayerHit(RifleHitResult, Player, false);
				}
				return;
			}
			// Enemy Casting
			if (AEnemy* Enemy = Cast<AEnemy>(RifleHitResult.GetActor()))
			{
				if (RifleHitResult.BoneName == FName("head"))
				{
					OnEnemyHit(RifleHitResult, Enemy, true);
				}
				else
				{
					OnEnemyHit(RifleHitResult, Enemy, false);
				}
				return;
			}
			// Reward Container Casting
			if (ARewardContainer* RewardContainer = Cast<ARewardContainer>(RifleHitResult.GetActor()))
			{
				OnContainerHit(RifleHitResult, RewardContainer);
			}
			// 지형지물에 적중
			else
			{
				OnGroundHit(RifleHitResult);
			}
		}
	}
}


void APlayerCharacter::ProcessSniperFire()
{
	if (curSniperAmmo > 0)
	{
		// Clamp를 통한 탄약 수 차감
		curSniperAmmo = FMath::Clamp(curSniperAmmo - 1, 0, 5);
		FVector StartLoc = FollowCamera->GetComponentLocation();
		FVector EndLoc = StartLoc + FollowCamera->GetForwardVector() * 10000.0f;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		// Perform Linetrace
		// 라인 트레이스가 적중했다면
		if (FHitResult SniperHitResult; GetWorld()->LineTraceSingleByChannel(SniperHitResult, StartLoc, EndLoc, ECC_Visibility, params))
		{
			// Player Character Casting
			// 플레이어 적중
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(SniperHitResult.GetActor()))
			{
				if (SniperHitResult.BoneName == FName("head"))
				{
					OnPlayerHit(SniperHitResult, Player, true);
				}
				else
				{
					OnPlayerHit(SniperHitResult, Player, false);
				}
				return;
			}
			// Enemy Casting
			if (AEnemy* Enemy = Cast<AEnemy>(SniperHitResult.GetActor()))
			{
				if (SniperHitResult.BoneName == FName("head"))
				{
					OnEnemyHit(SniperHitResult, Enemy, true);
				}
				else
				{
					OnEnemyHit(SniperHitResult, Enemy, false);
				}
				return;
			}
			// Reward Container Casting
			if (ARewardContainer* RewardContainer = Cast<ARewardContainer>(SniperHitResult.GetActor()))
			{
				OnContainerHit(SniperHitResult, RewardContainer);
			}
			else
			{
				OnGroundHit(SniperHitResult);
			}
		}
	}
}

void APlayerCharacter::ProcessSniperFireAnim()
{
	if (isZooming)
	{
		PlayAnimMontage(RifleFireMontage, 1, FName("SniperFire"));
	}
}

void APlayerCharacter::ProcessSniperFireLocal()
{
	UGameplayStatics::PlaySound2D(GetWorld(), SniperFireSound);
	const double RandF = UKismetMathLibrary::RandomFloatInRange(-0.7, -1.2);
	const double RandF2 = UKismetMathLibrary::RandomFloatInRange(-0.7, 0.8);
	AddControllerPitchInput(RandF);
	AddControllerYawInput(RandF2);
	if (isZooming)
	{
		const UE::Math::TVector<double> particleTrans = FollowCamera->GetComponentLocation() + FollowCamera->GetUpVector() * -70.0f;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SniperFireParticle, particleTrans);
		PC->PlayerCameraManager->StartCameraShake(sniperCameraShake);
	}
	else
	{
		FTransform particleTrans = SniperComp->GetSocketTransform(FName("SniperFirePosition"));
		particleTrans.SetScale3D(FVector(0.7));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SniperFireParticle, particleTrans);
		PC->PlayerCameraManager->StartCameraShake(sniperFireShake);
	}
}

void APlayerCharacter::ProcessSniperFireSimulatedProxy() const
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SniperFireSound, GetActorLocation());
	FTransform ParticleTrans = SniperComp->GetSocketTransform(FName("SniperFirePosition"));
	ParticleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SniperFireParticle, ParticleTrans);
}

void APlayerCharacter::ProcessPistolFire()
{
	if (curPistolAmmo > 0)
	{
		// Clamp를 통한 탄약 수 차감
		curPistolAmmo = FMath::Clamp(curPistolAmmo - 1, 0, 8);
		if (FirstPersonPistolComp->IsVisible())
		{
			PistolLineTraceStart = FirstPersonCamera->GetComponentLocation();
			PistolLineTraceEnd = PistolLineTraceStart + FirstPersonCamera->GetForwardVector() * 10000.0f;
		}
		else
		{
			PistolLineTraceStart = FollowCamera->GetComponentLocation();
			PistolLineTraceEnd = PistolLineTraceStart + FollowCamera->GetForwardVector() * 10000.0f;
		}
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		// Perform Linetrace
		if (FHitResult PistolHitResult; GetWorld()->LineTraceSingleByChannel(PistolHitResult, PistolLineTraceStart, PistolLineTraceEnd, ECC_Visibility, Params))
		{
			// Player Character Casting
			// 플레이어 적중
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(PistolHitResult.GetActor()))
			{
				if (PistolHitResult.BoneName == FName("head"))
				{
					OnPlayerHit(PistolHitResult, Player, true);
				}
				else
				{
					OnPlayerHit(PistolHitResult, Player, false);
				}
				return;
			}
			// Enemy Casting
			if (AEnemy* Enemy = Cast<AEnemy>(PistolHitResult.GetActor()))
			{
				if (PistolHitResult.BoneName == FName("head"))
				{
					OnEnemyHit(PistolHitResult, Enemy, true);
				}
				else
				{
					OnEnemyHit(PistolHitResult, Enemy, false);
				}
				return;
			}
			// Reward Container Casting
			if (ARewardContainer* RewardContainer = Cast<ARewardContainer>(PistolHitResult.GetActor()))
			{
				OnContainerHit(PistolHitResult, RewardContainer);
			}
			else
			{
				OnGroundHit(PistolHitResult);
			}
		}
	}
}

void APlayerCharacter::ProcessPistolFireAnim()
{
	if (isZooming)
	{
		PlayAnimMontage(FullBodyMontage, 1, FName("PistolZoomFire"));
	}
	else
	{
		PlayAnimMontage(FullBodyMontage, 1, FName("PistolFire"));
	}
}

void APlayerCharacter::ProcessPistolFireLocal()
{
	UGameplayStatics::PlaySound2D(GetWorld(), PistolFireSound);
	PC->PlayerCameraManager->StartCameraShake(pistolFireShake);
	if (FirstPersonCharacterMesh->IsVisible())
	{
		if (const auto FirstAnimInstance = Cast<UFirstPersonPlayerAnim>(FirstPersonCharacterMesh->GetAnimInstance()))
		{
			FirstAnimInstance->Montage_Play(FirstPersonPistolFireMontage, 1);
		}
		const FVector particleLoc = FirstPersonPistolComp->GetSocketLocation(FName("PistolFirePosition"));
		const UE::Math::TRotator<double> particleRot = FirstPersonPistolComp->GetSocketRotation(FName("PistolFirePosition"));
		const FTransform particleTrans = UKismetMathLibrary::MakeTransform(particleLoc, particleRot, FVector(0.4));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FirstPersonRifleFireParticle, particleTrans);
	}
	else
	{
		FTransform ParticleTrans = PistolComp->GetSocketTransform(FName("PistolFirePosition"));
		ParticleTrans.SetScale3D(FVector(0.7));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolfireParticle, ParticleTrans);
	}

	const double RandF = UKismetMathLibrary::RandomFloatInRange(-0.7, -1.2);
	const double RandF2 = UKismetMathLibrary::RandomFloatInRange(-0.7, 0.8);
	AddControllerPitchInput(RandF);
	AddControllerYawInput(RandF2);
}

void APlayerCharacter::ProcessPistolFireSimulatedProxy() const
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PistolFireSound, GetActorLocation());
	FTransform ParticleTrans = PistolComp->GetSocketTransform(FName("PistolFirePosition"));
	ParticleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolfireParticle, ParticleTrans);
}

void APlayerCharacter::ProcessM249Fire()
{
	if (curM249Ammo > 0)
	{
		// Clamp를 통한 탄약 수 차감
		curM249Ammo = FMath::Clamp(curM249Ammo - 1, 0, 100);
		FVector StartLoc = FollowCamera->GetComponentLocation();
		FVector EndLoc = StartLoc + FollowCamera->GetForwardVector() * 10000.0f;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		// Perform Linetrace
		if (FHitResult M249HitResult; GetWorld()->LineTraceSingleByChannel(M249HitResult, StartLoc, EndLoc, ECC_Visibility, params))
		{
			// Player Character Casting
			// 플레이어 적중
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(M249HitResult.GetActor()))
			{
				if (M249HitResult.BoneName == FName("head"))
				{
					OnPlayerHit(M249HitResult, Player, true);
				}
				else
				{
					OnPlayerHit(M249HitResult, Player, false);
				}
				return;
			}
			// Enemy Casting
			if (AEnemy* Enemy = Cast<AEnemy>(M249HitResult.GetActor()))
			{
				if (M249HitResult.BoneName == FName("head"))
				{
					OnEnemyHit(M249HitResult, Enemy, true);
				}
				else
				{
					OnEnemyHit(M249HitResult, Enemy, false);
				}
				return;
			}
			// Reward Container Casting
			if (ARewardContainer* RewardContainer = Cast<ARewardContainer>(M249HitResult.GetActor()))
			{
				OnContainerHit(M249HitResult, RewardContainer);
			}
			else
			{
				OnGroundHit(M249HitResult);
			}
		}
	}
}

void APlayerCharacter::ProcessM249FireAnim()
{
	PlayAnimMontage(FullBodyMontage, 1, FName("RifleFire"));
}

void APlayerCharacter::ProcessM249FireLocal()
{
	FTransform particleTrans = M249Comp->GetSocketTransform(FName("M249FirePosition"));
	particleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolfireParticle, particleTrans);
	UGameplayStatics::PlaySound2D(GetWorld(), M249FireSound);
	PC->PlayerCameraManager->StartCameraShake(rifleFireShake);
	if (isZooming)
	{
		const double RandF = UKismetMathLibrary::RandomFloatInRange(-0.4, -0.7);
		const double RandF2 = UKismetMathLibrary::RandomFloatInRange(-0.4, 0.4);
		AddControllerPitchInput(RandF);
		AddControllerYawInput(RandF2);
	}
	else
	{
		const double RandF = UKismetMathLibrary::RandomFloatInRange(-0.6, -1.1);
		const double RandF2 = UKismetMathLibrary::RandomFloatInRange(-0.5, 0.5);
		AddControllerPitchInput(RandF);
		AddControllerYawInput(RandF2);
	}
}

void APlayerCharacter::ProcessM249FireSimulatedProxy() const
{
	FTransform ParticleTrans = M249Comp->GetSocketTransform(FName("M249FirePosition"));
	ParticleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolfireParticle, ParticleTrans);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), M249FireSound, GetActorLocation());
}

float APlayerCharacter::SetFireInterval()
{
	if (weaponArray[0] == true)
	{
		return BulletsPerSecRifle;
	}
	if (weaponArray[1] == true)
	{
		return BulletsPerSecSniper;
	}
	if (weaponArray[2] == true)
	{
		return BulletsPerSecPistol;
	}
	if (weaponArray[3] == true)
	{
		return BulletsPerSecM249;
	}
	return 0;
}


void APlayerCharacter::PlayerDeath()
{
	PlayerDeathRPCServer();
}

void APlayerCharacter::PlayerDeathRPCServer_Implementation()
{
	PlayerDeathRPCMulticast();
}

bool APlayerCharacter::PlayerDeathRPCServer_Validate()
{
	return true;
}

void APlayerCharacter::PlayerDeathRPCMulticast_Implementation()
{
	// 몽타주 재생 중단
	StopAnimMontage();
	// 사망 몽타주 재생
	PlayAnimMontage(FullBodyMontage, 1, FName("Death"));
	IsPlayerDeadImmediately = true;
	FTimerHandle PlayerDeadHandle;
	GetWorld()->GetTimerManager().SetTimer(PlayerDeadHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		if (HasAuthority())
		{
			IsPlayerDead = true;
			GetMesh()->SetVisibility(false);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ADeadPlayerContainer* DeadPlayerBodyActor = GetWorld()->SpawnActor<ADeadPlayerContainer>(DeadPlayerContainerFactory, GetMesh()->GetComponentTransform(), Params);
			DeadPlayerContainerSettings(DeadPlayerBodyActor);
		}

		if (IsLocallyControlled())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), DeathSound);
		}

		GetMesh()->SetVisibility(false);
	}), 3.f, false);

	if (IsLocallyControlled())
	{
		SetFirstPersonModeRifle(false);
		SetFirstPersonModePistol(false);
		GetController()->SetIgnoreMoveInput(true);
		GetController()->SetIgnoreLookInput(true);
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
		APlayerCameraManager* playerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		// 카메라 페이드 연출
		playerCam->StartCameraFade(0, 1, 7.0, FLinearColor::Black, false, true);
		// 사망지점 전역변수에 캐싱
		DeathPosition = GetActorLocation();
	}


	// FTimerHandle endHandle;
	// // 7초 뒤 호출되는 함수 타이머
	// GetWorldTimerManager().SetTimer(endHandle, FTimerDelegate::CreateLambda([this]()->void
	// {
	// 	// 사망 변수 활성화
	// 	bPlayerDeath=true;
	// 	// 현재 주요 변수 값들을 GameInstance의 변수에 캐싱
	// 	CachingValues();
	// 	PouchCaching();
	// 	StashCaching();
	// 	GearCaching();
	// 	MagCaching();
	// 	ClearInventoryCache();
	// 	// 자신 제거
	// 	this->Destroy();
	// 	// 컨트롤러의 리스폰 함수 호출
	// 	PC->Respawn(this);	
	// 	}), 7.0f, false);
	// 	FTimerHandle possesHandle;
	// 	// 0.4초 뒤 호출되는 함수 타이머
	// 	GetWorld()->GetTimerManager().SetTimer(possesHandle, FTimerDelegate::CreateLambda([this]()->void
	// 	{
	// 		if (PC != nullptr)
	// 		{
	// 			// 리스폰 된 플레이어에 새롭게 빙의
	// 			PC->Possess(this);
	// 		}
	// }), 0.4f, false);
}
