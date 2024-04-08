// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Eclipse/Item/ArmorActor.h"
#include "Eclipse/UI/CrosshairWidget.h"
#include "Eclipse/UI/DamageWidget.h"
#include "Eclipse/UI/DamageWidgetActor.h"
#include "Eclipse/Game/EclipseGameInstance.h"
#include "Eclipse/Player/EclipsePlayerController.h"
#include "Eclipse/Enemy/Enemy.h"
#include "Eclipse/AI/EnemyFSM.h"
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
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/Game/EclipseGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Game/EclipsePlayerState.h"
#include "Eclipse/UI/ExtractionCountdown.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
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
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement=false;
	GetCharacterMovement()->bUseControllerDesiredRotation=false;

	// Configure character movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 150.0f, 0.0f); // ...at this rotation rate

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	sniperComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("sniperComp"));
	sniperComp->SetupAttachment(GetMesh(), FName("hand_r"));

	rifleComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("rifleComp"));
	rifleComp->SetupAttachment(GetMesh(), FName("hand_r"));

	pistolComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("pistolComp"));
	pistolComp->SetupAttachment(GetMesh(), FName("hand_l"));

	m249Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m249Comp"));
	m249Comp->SetupAttachment(GetMesh(), FName("hand_r"));

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


	bReplicates = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	this->SetActorEnableCollision(true);
	GetMesh()->HideBoneByName(TEXT("bot_hand"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(TEXT("shotgun_base"), EPhysBodyOp::PBO_None);

	IsPlayerDeadImmediately = false;
	IsPlayerDead = false;

	// Casting
	const UGameInstance* GI = GetGameInstance();
	PC = Cast<AEclipsePlayerController>(GI->GetFirstLocalPlayerController());
	gm = Cast<AEclipseGameMode>(GetWorld()->GetAuthGameMode());
	gi = Cast<UEclipseGameInstance>(GetWorld()->GetGameInstance());
	animInstance = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());

	//Add Input Mapping Context
	if (PC)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
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
	ExtractionCountdownUI=CreateWidget<UExtractionCountdown>(GetWorld(), ExtractionCountdownWidgetFactory);	

	if (IsLocallyControlled())
	{
		PC->SetAudioListenerOverride(GetMesh(), FVector::ZeroVector, FRotator::ZeroRotator);
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
		if (!crosshairUI->IsInViewport())
		{
			crosshairUI->AddToViewport();
		}

		APlayerCameraManager* const cameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
		cameraManager->StopCameraFade();
		cameraManager->StartCameraFade(1.0, 0, 8.0, FColor::Black, false, true);
	}

	ExtractionCountdownUI->ExtractionSuccessDele.AddUObject(this, &APlayerCharacter::ExtractionSuccess);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PlayerSpawnEmitter, GetActorLocation());

	if (PC)
	{
		PC->EnableInput(PC);
	}

	// Hideout
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) == FString("Safe_House"))
	{
		if (animInstance)
		{
			animInstance->bArmed = false;
		}
		GetCharacterMovement()->MaxWalkSpeed = 180.f;
		bUsingRifle = false;
		bUsingSniper = false;
		bUsingPistol = false;
		bUsingM249 = false;
		equippedWeaponStringArray.Add(FString("Empty")); //0
		equippedWeaponStringArray.Add(FString("Empty")); //1

		// Weapon Visibility Settings
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(false);

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
		rifleComp->SetVisibility(true);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(false);

		// Gear Visibility Settings
		GoggleSlot->SetVisibility(false);
		HelmetSlot->SetVisibility(false);
		HeadSetSlot->SetVisibility(false);
		MaskSlot->SetVisibility(false);
		ArmorSlot->SetVisibility(false);

		if (IsLocallyControlled())
		{
			if (informationUI)
			{
				FTimerHandle respawnTimer;
				GetWorldTimerManager().SetTimer(respawnTimer, FTimerDelegate::CreateLambda([this]()-> void
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
	}

	// Set Weapon Array
	weaponArray.Add(bUsingRifle); //0
	weaponArray.Add(bUsingSniper); //1
	weaponArray.Add(bUsingPistol); //2
	weaponArray.Add(bUsingM249); //3	

	// Apply Inventory Cache [GameInstance]
	ApplyCachingValues();
	ApplyPouchCache();
	ApplyInventoryCache();
	ApplyStashCache();
	ApplyGearCache();
	ApplyMagCache();

	// Update Tab Widget Before Widget Constructor
	UpdateTabWidget();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
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

void APlayerCharacter::Zoom()
{
	if (gi->IsWidgetOn || IsPlayerDeadImmediately)
	{
		return;
	}
	ZoomRPCServer();
}

void APlayerCharacter::ZoomRelease()
{
	if (gi->IsWidgetOn || IsPlayerDeadImmediately)
	{
		return;
	}
	ZoomRPCReleaseServer();
}

void APlayerCharacter::ZoomInput()
{
	Zoom();
	IsZoomKeyPressed = true;
}

void APlayerCharacter::ZoomReleaseInput()
{
	ZoomRelease();
	IsZoomKeyPressed = false;
}

void APlayerCharacter::ZoomRPCMulticast_Implementation()
{
	// Zooming Boolean
	isZooming = true;
	CharacterWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = 180.f;
	UPlayerAnim* const animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if (animInst)
	{
		animInst->bZooming = true;
	}
	// is using rifle
	if (weaponArray[0] == true)
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

		if (animInst)
		{
			animInst->bRifleZooming = true;
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
		if (animInst)
		{
			animInst->bRifleZooming = true;
		}
	}
	else if (weaponArray[2] == true)
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
		if (animInst)
		{
			animInst->bM249Zooming = true;
		}
	}
}

void APlayerCharacter::ZoomRPCServer_Implementation()
{
	ZoomRPCMulticast();
}

bool APlayerCharacter::ZoomRPCServer_Validate()
{
	return true;
}


void APlayerCharacter::ZoomRPCReleaseServer_Implementation()
{
	ZoomRPCReleaseMulticast();
}

bool APlayerCharacter::ZoomRPCReleaseServer_Validate()
{
	return true;
}

void APlayerCharacter::ZoomRPCReleaseMulticast_Implementation()
{
	// Zooming Boolean
	isZooming = false;
	GetCharacterMovement()->MaxWalkSpeed = CharacterWalkSpeed;
	UPlayerAnim* const animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if (animInst)
	{
		animInst->bZooming = false;
	}
	if (weaponArray[0] == true)
	{
		if (animInst)
		{
			animInst->bRifleZooming = false;
		}
		Timeline.ReverseFromEnd();
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
		if (animInst)
		{
			animInst->bRifleZooming = false;
		}
	}
	else if (weaponArray[3] == true)
	{
		if (animInst)
		{
			animInst->bM249Zooming = false;
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
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != FString("Safe_House"))
	{
		RunRPCServer();
	}
}

void APlayerCharacter::RunRelease()
{
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != FString("Safe_House"))
	{
		RunRPCReleaseServer();
	}
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
	UPlayerAnim* animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if (animInst)
	{
		bool montagePlaying = animInst->IsAnyMontagePlaying();
		if (montagePlaying)
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
		rifleComp->SetVisibility(true);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(false);
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
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(true);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(false);
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
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(true);
		m249Comp->SetVisibility(false);
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
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(true);
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
	UPlayerAnim* animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if (animInst)
	{
		bool montagePlaying = animInst->IsAnyMontagePlaying();
		if (montagePlaying)
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
		rifleComp->SetVisibility(true);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(false);
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
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(true);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(false);
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
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(true);
		m249Comp->SetVisibility(false);
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
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(true);
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
			HitCharacter->Damaged(GetAttackDamage(true) * 2, this);
			Stat->AccumulatedDamageToPlayer += GetAttackDamageCache(true) * 2;
		}
		else
		{
			HitCharacter->Damaged(GetAttackDamage(true), this);
			Stat->AccumulatedDamageToPlayer += GetAttackDamageCache(true);
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
			SetDamageWidget(GetAttackDamageCache(true) * 2, HitResult.Location, false, FLinearColor::Yellow);
			// 적중 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticle, HitResult.Location, hitRot, FVector(1.f));
		}
		else
		{
			UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
			// 적중 위젯 애니메이션 재생
			crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
			// 데미지 위젯에 피해 값과 적 위치벡터 할당
			SetDamageWidget(GetAttackDamageCache(true), HitResult.Location, false, FLinearColor::White);
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
	if(HasAuthority())
	{
		if (weaponArray[0])	maxRifleAmmo+=20;
		else if (weaponArray[1]) maxSniperAmmo+=4;
		else if (weaponArray[2]) maxPistolAmmo+=6;
		else if (weaponArray[3]) maxM249Ammo+=30;
	}
	if (IsLocallyControlled())
	{
		crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
		UGameplayStatics::PlaySound2D(GetWorld(), KillSound);
		informationUI->ChargeAmmunitionInfoWidget();
		informationUI->PlayAnimation(informationUI->ChargeAmmunition);
		informationUI->UpdateAmmo_Secondary();
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

void APlayerCharacter::OnEnemyHitRPCMulticast_Implementation(const FHitResult& HitResult, AEnemy* HitEnemy, bool IsHeadshot)
{
	if (HasAuthority())
	{
		if (IsHeadshot)
		{
			HitEnemy->Damaged(GetAttackDamage(false) * 2, this);
			Stat->AccumulatedDamageToEnemy += GetAttackDamageCache(false) * 2;
		}
		else
		{
			HitEnemy->Damaged(GetAttackDamage(false), this);
			Stat->AccumulatedDamageToEnemy += GetAttackDamageCache(false);
		}
	}
	if (IsLocallyControlled())
	{
		const FRotator hitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		if (HitEnemy->EnemyStat->IsStunned)
		{
			if (IsHeadshot)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), BulletHeadHitSound);
				// 적중 위젯 애니메이션 재생
				crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
				// 데미지 위젯에 피해 값과 적 위치벡터 할당
				SetDamageWidget(GetAttackDamageCache(false) * 4, HitResult.Location, false, FLinearColor::Red);
				// 적중 파티클 스폰
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, hitRot, FVector(2.f));
			}
			else
			{
				UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
				// 적중 위젯 애니메이션 재생
				crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
				// 데미지 위젯에 피해 값과 적 위치벡터 할당
				SetDamageWidget(GetAttackDamageCache(false) * 2, HitResult.Location, false, FLinearColor::Red);
				// 적중 파티클 스폰
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, hitRot, FVector(1.f));
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
					SetDamageWidget(GetAttackDamageCache(false) * 2, HitResult.Location, false, FLinearColor::Yellow);
					// 적중 파티클 스폰
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, hitRot, FVector(2.f));
				}
				else
				{
					UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
					// 적중 위젯 애니메이션 재생
					crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
					// 데미지 위젯에 피해 값과 적 위치벡터 할당
					SetDamageWidget(GetAttackDamageCache(false), HitResult.Location, false, FLinearColor::White);
					// 적중 파티클 스폰
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, hitRot, FVector(1.f));
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
					SetDamageWidget(GetAttackDamageCache(false) * 0.1f, HitResult.Location, true, FLinearColor::Gray);
					// 적중 파티클 스폰
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, hitRot, FVector(2.f));
				}
				else
				{
					UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
					// 적중 위젯 애니메이션 재생
					crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
					// 데미지 위젯에 피해 값과 적 위치벡터 할당
					SetDamageWidget(GetAttackDamageCache(false) * 0.05f, HitResult.Location, true, FLinearColor::Gray);
					// 적중 파티클 스폰
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, hitRot, FVector(1.f));
				}
			}
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
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, hitRot, FVector(2.f));
		}
		else
		{
			// 적중 사운드 재생
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, HitResult.Location);
			// 적중 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, hitRot, FVector(1.f));
		}
	}
	// // Enemy FSM Casting
	// UEnemyFSM* fsm = Cast<UEnemyFSM>(HitEnemy->GetDefaultSubobjectByName(FName("enemyFSM")));
	// // Check Enemy Type
	// const AGuardian* guardian = Cast<AGuardian>(HitEnemy);
	// const ACrunch* crunch = Cast<ACrunch>(HitEnemy);
	// if (guardian)
	// {
	// 	bGuardian = true;
	// }
	// else if (crunch)
	// {
	// 	bCrunch = true;
	// }
	// // 이미 죽지 않은 적에게만 실행
	// if (HitEnemy->bDeath == false)
	// {
	// 	hitActors = HitResult.GetActor();
	// 	FName hitBone = HitResult.BoneName;
	// 	FVector_NetQuantize hitLoc = HitResult.Location;
	// 	FRotator hitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
	// 	// 헤드샷 적중
	// 	if (hitBone == FName("head"))
	// 	{
	// 		// 반환값 float의 데미지 증가 처리 함수와 곱연산
	// 		float min = FMath::RoundFromZero(120 * DamageMultiplier());
	// 		float max = FMath::RoundFromZero(180 * DamageMultiplier());
	// 		// 헤드 데미지 랜덤 산출
	// 		randRifleHeadDamage = FMath::RandRange(min, max);
	// 		// 이번 공격에 적이 죽는다면
	// 		if (HitEnemy->EnemyStat->GetCurrentHp() <= randRifleHeadDamage)
	// 		{
	// 			// Enemy Kill 위젯 애니메이션 재생
	// 			crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
	// 			// 킬 사운드 재생
	// 			UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
	// 			// 킬 파티클 스폰
	// 			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.0f));
	// 			// FSM에 있는 Damage Process 호출		
	// 			fsm->OnDamageProcess(randRifleHeadDamage);
	// 			SetDamageWidget(randRifleHeadDamage, hitLoc, false, FLinearColor::Yellow);
	// 			// 헤드 적중 데미지 프로세스 호출
	// 			HitEnemy->OnHeadDamaged();
	// 			// 사망 불리언 활성화
	// 			HitEnemy->bDeath = true;
	// 			// if Guardian Kill
	// 			if (bGuardian)
	// 			{
	// 				GuardianCount++;
	// 				informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
	// 				// 전리품 드롭
	// 				HitEnemy->DropReward();
	// 			}
	// 			// if Crunch Kill
	// 			else if (bCrunch)
	// 			{
	// 				BossCount++;
	// 				informationUI->BossCount->SetText(FText::AsNumber(BossCount));
	// 				SetBossHPWidget(HitEnemy);
	// 				HitEnemy->DropReward();
	// 				FTimerHandle removeHandle;
	// 				GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
	// 			}
	// 		}
	// 		// 이번 공격에 적이 죽지 않는다면
	// 		else
	// 		{
	// 			// 헤드 적중 위젯 애니메이션 재생
	// 			crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
	// 			// 헤드 적중 사운드 재생
	// 			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHeadHitSound, hitLoc);
	// 			// 헤드 적중 파티클 스폰
	// 			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.0f));
	// 			// 적중 대상이 보스라면
	// 			if (bCrunch)
	// 			{
	// 				// 실드가 파괴된 상태라면
	// 				if (HitEnemy->EnemyStat->isShieldBroken)
	// 				{
	// 					// FSM에 있는 Damage Process 호출		
	// 					fsm->OnDamageProcess(randRifleHeadDamage);
	// 					if (HitEnemy->isStunned)
	// 					{
	// 						// 데미지 위젯에 피해 값과 적 위치벡터 할당
	// 						SetDamageWidget(randRifleHeadDamage * 2, hitLoc, false, FLinearColor::Red);
	// 					}
	// 					else
	// 					{
	// 						// 데미지 위젯에 피해 값과 적 위치벡터 할당
	// 						SetDamageWidget(randRifleHeadDamage, hitLoc, false, FLinearColor::Yellow);
	// 					}
	// 					// 헤드 적중 데미지 프로세스 호출
	// 					HitEnemy->OnHeadDamaged();
	// 				}
	// 				// 실드가 파괴되지 않은 상태라면
	// 				else
	// 				{
	// 					FTransform EmitterTrans = HitEnemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
	// 					EmitterTrans.SetScale3D(FVector(1.3));
	// 					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
	// 					// FSM에 있는 Damage Process 호출		
	// 					fsm->OnShieldDamageProcess(randRifleHeadDamage);
	// 					// 데미지 위젯에 피해 값과 적 위치벡터 할당
	// 					SetDamageWidget(randRifleHeadDamage / 20, hitLoc, true, FLinearColor::Yellow);
	// 					// 헤드 적중 데미지 프로세스 호출
	// 					HitEnemy->OnHeadDamaged();
	// 				}
	// 				SetBossHPWidget(HitEnemy);
	// 			}
	// 			// 보스가 아니라면
	// 			else
	// 			{
	// 				// FSM에 있는 Damage Process 호출		
	// 				fsm->OnDamageProcess(randRifleHeadDamage);
	//
	// 				// 헤드 적중 데미지 프로세스 호출
	// 				HitEnemy->OnHeadDamaged();
	// 			}
	// 		}
	// 	}
	// 	// 일반 적중
	// 	else
	// 	{
	// 		// 반환값 float의 데미지 증가 처리 함수와 곱연산
	// 		float min = FMath::RoundFromZero(60 * DamageMultiplier());
	// 		float max = FMath::RoundFromZero(90 * DamageMultiplier());
	// 		// 일반 데미지 랜덤 산출
	// 		randRifleDamage = FMath::RandRange(min, max);
	// 		// 이번 공격에 적이 죽는다면
	// 		if (HitEnemy->EnemyStat->GetCurrentHp() <= randRifleDamage)
	// 		{
	// 			// Enemy Kill 위젯 애니메이션 재생
	// 			crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
	// 			// 킬 사운드 재생
	// 			UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
	// 			// 킬 파티클 스폰
	// 			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.0f));
	// 			// FSM에 있는 Damage Process 호출		
	// 			fsm->OnDamageProcess(randRifleDamage);
	// 			SetDamageWidget(randRifleDamage, hitLoc, false, FLinearColor::White);
	// 			// 일반 적중 데미지 프로세스 호출
	// 			HitEnemy->OnDamaged();
	//
	// 			// 사망 불리언 활성화
	// 			HitEnemy->bDeath = true;
	// 			if (bGuardian)
	// 			{
	// 				GuardianCount++;
	// 				informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
	// 				// 전리품 드롭
	// 				HitEnemy->DropReward();
	// 			}
	// 			else if (bCrunch)
	// 			{
	// 				BossCount++;
	// 				informationUI->BossCount->SetText(FText::AsNumber(BossCount));
	// 				SetBossHPWidget(HitEnemy);
	// 				// 전리품 드롭
	// 				HitEnemy->DropReward();
	// 				FTimerHandle removeHandle;
	// 				GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
	// 			}
	// 		}
	// 		else
	// 		{
	// 			// 적중 위젯 애니메이션 재생
	// 			crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
	// 			// 적중 사운드 재생
	// 			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, hitLoc);
	// 			// 적중 파티클 스폰
	// 			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(0.5f));
	// 			// 적중 대상이 보스라면
	// 			if (bCrunch)
	// 			{
	// 				// 실드가 파괴된 상태라면
	// 				if (HitEnemy->EnemyStat->isShieldBroken)
	// 				{
	// 					// FSM에 있는 Damage Process 호출		
	// 					fsm->OnDamageProcess(randRifleDamage);
	// 					if (HitEnemy->isStunned)
	// 					{
	// 						// 데미지 위젯에 피해 값과 적 위치벡터 할당
	// 						SetDamageWidget(randRifleDamage * 2, hitLoc, false, FLinearColor::Red);
	// 					}
	// 					else
	// 					{
	// 						// 데미지 위젯에 피해 값과 적 위치벡터 할당
	// 						SetDamageWidget(randRifleDamage, hitLoc, false, FLinearColor::White);
	// 					}
	// 					// 일반 적중 데미지 프로세스 호출
	// 					HitEnemy->OnDamaged();
	// 				}
	// 				// 실드가 있는 상태라면
	// 				else
	// 				{
	// 					FTransform EmitterTrans = HitEnemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
	// 					EmitterTrans.SetScale3D(FVector(1.3));
	// 					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
	// 					// FSM에 있는 Damage Process 호출		
	// 					fsm->OnShieldDamageProcess(randRifleDamage);
	// 					// 데미지 위젯에 피해 값과 적 위치벡터 할당
	// 					SetDamageWidget(randRifleDamage / 20, hitLoc, true, FLinearColor::White);
	// 					// 일반 적중 데미지 프로세스 호출
	// 					HitEnemy->OnDamaged();
	// 				}
	// 				SetBossHPWidget(HitEnemy);
	// 			}
	// 		}
	// 	}
	// }
	// bGuardian = false;
	// bCrunch = false;
}


void APlayerCharacter::OnGroundHit(const FHitResult& HitResult)
{
	OnGroundHitRPCServer(HitResult);
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
	if(HasAuthority())
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
	if(IsLocallyControlled())
	{
		const FRotator hitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, hitRot, FVector(1.f));
		UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
	}
	else
	{
		const FRotator hitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, hitRot, FVector(1.f));
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, HitContainer->GetActorLocation());
	}
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
		const FRotator decalRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		const FVector_NetQuantize decalLoc = HitResult.Location;
		const FTransform decalTrans = UKismetMathLibrary::MakeTransform(decalLoc, decalRot);
		GetWorld()->SpawnActor<AActor>(ShotDecalFactory, decalTrans);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, decalLoc, decalRot + FRotator(-90, 0, 0), FVector(0.5f));
	}
	else
	{
		const FRotator decalRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		const FVector_NetQuantize decalLoc = HitResult.Location;
		const FTransform decalTrans = UKismetMathLibrary::MakeTransform(decalLoc, decalRot);
		GetWorld()->SpawnActor<AActor>(ShotDecalFactory, decalTrans);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, decalLoc, decalRot + FRotator(-90, 0, 0), FVector(0.5f));
	}
}

int32 APlayerCharacter::GenerateRandomDamage(float InDamage)
{
	const double DoubleRandDamage = FMath::FRandRange(InDamage * 0.8, InDamage * 1.2);
	const int32 RoundedRandDamage = FMath::RoundHalfToEven(DoubleRandDamage);
	return RoundedRandDamage;
}


void APlayerCharacter::Tab()
{
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
		FHitResult actorHitResult;
		FVector StartLoc = FollowCamera->GetComponentLocation();
		FVector EndLoc = StartLoc + FollowCamera->GetForwardVector() * 500.0f;
		// 무기 액터 탐지 라인 트레이스
		bool bHit = GetWorld()->LineTraceSingleByChannel(actorHitResult, StartLoc, EndLoc, ECC_Visibility);
		if (bHit)
		{
			// 무기 액터 캐스팅
			rifleActor = Cast<ARifleActor>(actorHitResult.GetActor());
			sniperActor = Cast<ASniperActor>(actorHitResult.GetActor());
			pistolActor = Cast<APistolActor>(actorHitResult.GetActor());
			m249Actor = Cast<AM249Actor>(actorHitResult.GetActor());
			MissionChecker = Cast<AMissionChecker>(actorHitResult.GetActor());
			PickableItemActor = Cast<APickableActor>(actorHitResult.GetActor());
			StageBoard = Cast<AStageBoard>(actorHitResult.GetActor());
			Stash = Cast<AStash>(actorHitResult.GetActor());
			QuitGameActor = Cast<AQuitGameActor>(actorHitResult.GetActor());
			PlayerCharacter = Cast<APlayerCharacter>(actorHitResult.GetActor());

			// 라이플 탐지
			if (rifleActor)
			{
				// 1회 실행 불리언
				if (TickOverlapBoolean == false)
				{
					TickOverlapBoolean = true;
					isCursorOnRifle = true;
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
					isCursorOnSniper = true;
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
					isCursorOnPistol = true;
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
					isCursorOnM249 = true;
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
			else if (PlayerCharacter)
			{
				if (PlayerCharacter->IsPlayerDead)
				{
					// 1회 실행 불리언
					if (TickOverlapBoolean == false)
					{
						TickOverlapBoolean = true;
						// Render Custom Depth 활용한 무기 액터 외곽선 활성화
						PlayerCharacter->GetMesh()->SetRenderCustomDepth(true);
						// Widget Switcher 이용한 무기 정보 위젯 스위칭
						infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(19);
						// Radial Slider Value 초기화
						infoWidgetUI->weaponHoldPercent = 0;
						// Weapon Info Widget 뷰포트에 배치
						infoWidgetUI->AddToViewport();
					}
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
					bool bEndOverlapHit = GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(500), params);
					if (bEndOverlapHit)
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
							StageBoard = Cast<AStageBoard>(HitObj[i].GetActor());
							Stash = Cast<AStash>(HitObj[i].GetActor());
							QuitGameActor = Cast<AQuitGameActor>(HitObj[i].GetActor());
							PlayerCharacter = Cast<APlayerCharacter>(HitObj[i].GetActor());

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
							else if (QuitGameActor)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								QuitGameActor->quitGameMesh->SetRenderCustomDepth(false);
							}
							else if (PlayerCharacter)
							{
								// Render Custom Depth 활용한 무기 액터 외곽선 해제
								PlayerCharacter->GetMesh()->SetRenderCustomDepth(false);
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
	DoorInteractionDele.ExecuteIfBound();
}

void APlayerCharacter::SetBossHPWidget(const AEnemy* enemy)
{
	if (enemy && bossHPUI)
	{
		const float bossHP = enemy->EnemyStat->GetCurrentHp() * 0.0001;
		bossHPUI->progressBar->SetPercent(bossHP);
		const float bossShield = enemy->EnemyStat->GetCurrentShield() * 0.01;
		bossHPUI->shieldProgressBar->SetPercent(bossShield);
	}
}

void APlayerCharacter::SetDamageWidget(int damage, FVector spawnLoc, bool isShieldIconEnable, FLinearColor DamageTextColor)
{
	ADamageWidgetActor* damWidget = GetWorld()->SpawnActor<ADamageWidgetActor>(damageWidgetFactory, spawnLoc + FVector(0, 0, 50), FRotator::ZeroRotator);
	if (damWidget)
	{
		UUserWidget* widui = damWidget->DamageWidgetComponent->GetUserWidgetObject();
		if (widui)
		{
			damageWidgetUI = Cast<UDamageWidget>(widui);
			if (damageWidgetUI)
			{
				damageWidgetUI->damageText->SetColorAndOpacity(DamageTextColor);
				damageWidgetUI->damage = damage;
				if (isShieldIconEnable)
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
	if (PickableActor)
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
				SetActorTickEnabled(false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();
				RifleMagActor->AddInventory(this);
			}
			if (HasAuthority())
			{
				RifleMagActor->SetActorHiddenInGame(true);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					RifleMagActor->Destroy();
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			return;
		}
		SniperMagActor = Cast<ASniperMagActor>(PickableActor);
		if (SniperMagActor)
		{
			if (HasAuthority())
			{
				SetActorTickEnabled(false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();

				SniperMagActor->AddInventory(this);
			}
			if (HasAuthority())
			{
				SniperMagActor->SetActorHiddenInGame(true);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					SniperMagActor->Destroy();
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			return;
		}
		PistolMagActor = Cast<APistolMagActor>(PickableActor);
		if (PistolMagActor)
		{
			if (HasAuthority())
			{
				SetActorTickEnabled(false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();

				PistolMagActor->AddInventory(this);
			}
			if (HasAuthority())
			{
				PistolMagActor->SetActorHiddenInGame(true);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					PistolMagActor->Destroy();
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			return;
		}
		M249MagActor = Cast<AM249MagActor>(PickableActor);
		if (M249MagActor)
		{
			if (HasAuthority())
			{
				SetActorTickEnabled(false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();

				M249MagActor->AddInventory(this);
			}
			if (HasAuthority())
			{
				M249MagActor->SetActorHiddenInGame(true);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					M249MagActor->Destroy();
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			return;
		}
		GoggleActor = Cast<AGoggleActor>(PickableActor);
		if (GoggleActor)
		{
			if (HasAuthority())
			{
				SetActorTickEnabled(false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();

				GoggleActor->AddInventory(this);
			}
			if (HasAuthority())
			{
				GoggleActor->SetActorHiddenInGame(true);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					GoggleActor->Destroy();
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			return;
		}
		HelmetActor = Cast<AHelmetActor>(PickableActor);
		if (HelmetActor)
		{
			if (HasAuthority())
			{
				SetActorTickEnabled(false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();

				HelmetActor->AddInventory(this);
			}
			if (HasAuthority())
			{
				HelmetActor->SetActorHiddenInGame(true);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					HelmetActor->Destroy();
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			return;
		}
		HeadsetActor = Cast<AHeadsetActor>(PickableActor);
		if (HeadsetActor)
		{
			if (HasAuthority())
			{
				SetActorTickEnabled(false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();

				HeadsetActor->AddInventory(this);
			}
			if (HasAuthority())
			{
				HeadsetActor->SetActorHiddenInGame(true);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					HeadsetActor->Destroy();
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			return;
		}
		MaskActor = Cast<AMaskActor>(PickableActor);
		if (MaskActor)
		{
			if (HasAuthority())
			{
				SetActorTickEnabled(false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();

				MaskActor->AddInventory(this);
			}
			if (HasAuthority())
			{
				MaskActor->SetActorHiddenInGame(true);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					MaskActor->Destroy();
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			return;
		}
		ArmorActor = Cast<AArmorActor>(PickableActor);
		if (ArmorActor)
		{
			if (HasAuthority())
			{
				SetActorTickEnabled(false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();

				ArmorActor->AddInventory(this);
			}
			if (HasAuthority())
			{
				ArmorActor->SetActorHiddenInGame(true);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					ArmorActor->Destroy();
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			return;
		}
		MedKitActor = Cast<AMedKitActor>(PickableActor);
		if (MedKitActor)
		{
			if (HasAuthority())
			{
				SetActorTickEnabled(false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();

				MedKitActor->AddInventory(this);
			}
			if (HasAuthority())
			{
				MedKitActor->SetActorHiddenInGame(true);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					MedKitActor->Destroy();
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			return;
		}
		HackingConsole = Cast<AHackingConsole>(PickableActor);
		if (HackingConsole)
		{
			if (HasAuthority())
			{
				SetActorTickEnabled(false);
			}
			if (IsLocallyControlled())
			{
				if (infoWidgetUI->IsInViewport()) infoWidgetUI->RemoveFromParent();

				HackingConsole->AddInventory(this);
				ConsoleCount++;
				informationUI->ConsoleCount->SetText(FText::AsNumber(ConsoleCount));
			}
			if (HasAuthority())
			{
				HackingConsole->SetActorHiddenInGame(true);
				GetWorldTimerManager().SetTimer(DestroyHandle, FTimerDelegate::CreateLambda([this]()-> void
				{
					HackingConsole->Destroy();
					SetActorTickEnabled(true);
				}), 1.f, false);
			}
			return;
		}
	}
}


void APlayerCharacter::DeadBodyInteraction(APlayerCharacter* DeadPlayerCharacter)
{
	DeadBodyInteractionRPCServer(DeadPlayerCharacter);
}

void APlayerCharacter::DeadBodyInteractionRPCServer_Implementation(APlayerCharacter* DeadPlayerCharacter)
{
	DeadBodyInteractionRPCMutlicast(DeadPlayerCharacter);
}

bool APlayerCharacter::DeadBodyInteractionRPCServer_Validate(APlayerCharacter* DeadPlayerCharacter)
{
	return true;
}

void APlayerCharacter::DeadBodyInteractionRPCMutlicast_Implementation(APlayerCharacter* DeadPlayerCharacter)
{
	if (IsLocallyControlled())
	{
		if (AEclipsePlayerState* ECDeadPlayerState = Cast<AEclipsePlayerState>(DeadPlayerCharacter->GetPlayerState()))
		{
			ECDeadPlayerState->DeadBodyWidgetSettings(this, ECDeadPlayerState);
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
	rifleComp->SetVisibility(true);
	sniperComp->SetVisibility(false);
	pistolComp->SetVisibility(false);
	m249Comp->SetVisibility(false);
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
	rifleComp->SetVisibility(false);
	sniperComp->SetVisibility(true);
	pistolComp->SetVisibility(false);
	m249Comp->SetVisibility(false);

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
	rifleComp->SetVisibility(false);
	sniperComp->SetVisibility(false);
	pistolComp->SetVisibility(true);
	m249Comp->SetVisibility(false);

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
	rifleComp->SetVisibility(false);
	sniperComp->SetVisibility(false);
	pistolComp->SetVisibility(false);
	m249Comp->SetVisibility(true);

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
	bool bHit = GetWorld()->LineTraceSingleByChannel(actorHitResult, StartLoc, EndLoc, ECC_Visibility);
	if (bHit)
	{
		// 무기 액터 캐스팅
		rifleActor = Cast<ARifleActor>(actorHitResult.GetActor());
		sniperActor = Cast<ASniperActor>(actorHitResult.GetActor());
		pistolActor = Cast<APistolActor>(actorHitResult.GetActor());
		m249Actor = Cast<AM249Actor>(actorHitResult.GetActor());

		PickableItemActor = Cast<APickableActor>(actorHitResult.GetActor());

		StageBoard = Cast<AStageBoard>(actorHitResult.GetActor());
		Stash = Cast<AStash>(actorHitResult.GetActor());
		QuitGameActor = Cast<AQuitGameActor>(actorHitResult.GetActor());

		APlayerCharacter* EnemyCharacter = Cast<APlayerCharacter>(actorHitResult.GetActor());

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
						PouchCaching();
						if (AEclipsePlayerState* CachingPlayerState = Cast<AEclipsePlayerState>(GetPlayerState()))
						{
							CachingPlayerState->InventoryCaching(this);
						}
						GearCaching();
						MagCaching();
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
		else if (EnemyCharacter)
		{
			// 키다운 시간 동안 Radial Slider 게이지 상승
			infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
			if (EnemyCharacter->IsPlayerDead)
			{
				if (quitWidgetUI && infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
				{
					infoWidgetUI->weaponHoldPercent = 0;
					if (bDeadBodyWidgetOn == false)
					{
						bDeadBodyWidgetOn = true;
						DeadBodyInteraction(EnemyCharacter);
					}
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
	const bool IsMontagePlaying = animInstance->IsAnyMontagePlaying();
	if (!IsMontagePlaying)
	{
		if (weaponArray[0] == true && curRifleAmmo < 40 + SetRifleAdditionalMagazine() && maxRifleAmmo > 0)
		{
			if (IsLocallyControlled())
			{
				crosshairUI->PlayAnimation(crosshairUI->ReloadAnimation);
				UGameplayStatics::PlaySound2D(GetWorld(), RifleReloadSound);
			}
			else
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), RifleReloadSound, GetActorLocation());
			}

			PlayAnimMontage(UpperOnlyMontage, 1, FName("Reload"));
		}
		else if (weaponArray[1] == true && curSniperAmmo < 5 + SetSniperAdditionalMagazine() && maxSniperAmmo > 0)
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
		else if (weaponArray[2] == true && curPistolAmmo < 8 + SetPistolAdditionalMagazine() && maxPistolAmmo > 0)
		{
			if (IsLocallyControlled())
			{
				crosshairUI->PlayAnimation(crosshairUI->ReloadAnimation);
				UGameplayStatics::PlaySound2D(GetWorld(), PistolReloadSound);
			}
			else
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), PistolReloadSound, GetActorLocation());
			}
			PlayAnimMontage(UpperOnlyMontage, 1, FName("PistolReload"));
		}
		else if (weaponArray[3] == true && curM249Ammo < 100 + SetM249AdditionalMagazine() && maxM249Ammo > 0)
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
		PouchCaching();
		if (AEclipsePlayerState* CachingPlayerState = Cast<AEclipsePlayerState>(GetPlayerState())) CachingPlayerState->InventoryCaching(this);
		StashCaching();
		GearCaching();
		MagCaching();
		UGameplayStatics::OpenLevel(GetWorld(), FName("Map_BigStarStation"));
	}), 9.f, false);
}

void APlayerCharacter::SetZoomValue(float Value)
{
	if (weaponArray[1] == true && !SniperZoomBool && !SniperZoomOutBool)
	{
		// 타임라인 Float Curve 에 따른 Lerp
		double lerp = UKismetMathLibrary::Lerp(90, 40, Value);
		// 해당 Lerp값 Arm Length에 적용
		FollowCamera->SetFieldOfView(lerp);
	}
	else if (weaponArray[1] == false)
	{
		// 타임라인 Float Curve 에 따른 Lerp
		double lerp = UKismetMathLibrary::Lerp(200, 100, Value);
		// 해당 Lerp값 Arm Length에 적용
		CameraBoom->TargetArmLength = lerp;
	}
	else if (SniperZoomBool)
	{
		double lerp = UKismetMathLibrary::Lerp(40, 20, Value);
		FollowCamera->SetFieldOfView(lerp);
		GetWorldTimerManager().SetTimer(SniperZoomHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			SniperZoomBool = false;
		}), 1.0f, false);
	}
	else if (SniperZoomOutBool)
	{
		float lerp = FollowCamera->FieldOfView = UKismetMathLibrary::Lerp(20, 40, Value);
		FollowCamera->SetFieldOfView(lerp);
		GetWorldTimerManager().SetTimer(SniperZoomOutHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			SniperZoomOutBool = false;
		}), 1.0f, false);
	}
}

void APlayerCharacter::CachingValues()
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

void APlayerCharacter::Damaged(int damage, AActor* DamageCauser)
{
	DamagedRPCServer(damage, DamageCauser);
	Stat->ApplyDamage(damage, DamageCauser);
}

void APlayerCharacter::DamagedRPCServer_Implementation(int damage, AActor* DamageCauser)
{
	DamagedRPCMulticast(damage, DamageCauser);
}

bool APlayerCharacter::DamagedRPCServer_Validate(int damage, AActor* DamageCauser)
{
	return true;
}

void APlayerCharacter::DamagedRPCMulticast_Implementation(int damage, AActor* DamageCauser)
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

int32 APlayerCharacter::SetRifleAdditionalMagazine()
{
	if (bRifleAdditionalMag)
	{
		return 15;
	}
	return 0;
}

int32 APlayerCharacter::SetSniperAdditionalMagazine()
{
	if (bSniperAdditionalMag)
	{
		return 2;
	}
	return 0;
}

int32 APlayerCharacter::SetPistolAdditionalMagazine()
{
	if (bPistolAdditionalMag)
	{
		return 4;
	}
	return 0;
}

int32 APlayerCharacter::SetM249AdditionalMagazine()
{
	if (bM249AdditionalMag)
	{
		return 30;
	}
	return 0;
}

void APlayerCharacter::SetRifleAdditionalMagazineSlot()
{
	bRifleAdditionalMag = true;
}

void APlayerCharacter::SetSniperAdditionalMagazineSlot()
{
	bSniperAdditionalMag = true;
}

void APlayerCharacter::SetPistolAdditionalMagazineSlot()
{
	bPistolAdditionalMag = true;
}

void APlayerCharacter::SetM249AdditionalMagazineSlot()
{
	bM249AdditionalMag = true;
}

void APlayerCharacter::UnSetRifleAdditionalMagazineSlot()
{
	bRifleAdditionalMag = false;
	if (curRifleAmmo >= 15)
	{
		curRifleAmmo -= 15;
		maxRifleAmmo += 15;
	}
	else
	{
		curRifleAmmo = 0;
		maxRifleAmmo += curRifleAmmo;
	}
}

void APlayerCharacter::UnSetSniperAdditionalMagazineSlot()
{
	bSniperAdditionalMag = false;
}

void APlayerCharacter::UnSetPistolAdditionalMagazineSlot()
{
	bPistolAdditionalMag = false;
}

void APlayerCharacter::UnSetM249AdditionalMagazineSlot()
{
	bM249AdditionalMag = false;
}

void APlayerCharacter::OnRep_WeaponArrayChanged()
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
	DOREPLIFETIME(APlayerCharacter, RandPlayerAttackDamageRifle);
	DOREPLIFETIME(APlayerCharacter, RandPlayerAttackDamagePistol);
	DOREPLIFETIME(APlayerCharacter, RandPlayerAttackDamageSniper);
	DOREPLIFETIME(APlayerCharacter, RandPlayerAttackDamageM249);
	DOREPLIFETIME(APlayerCharacter, RandEnemyAttackDamageRifle);
	DOREPLIFETIME(APlayerCharacter, RandEnemyAttackDamagePistol);
	DOREPLIFETIME(APlayerCharacter, RandEnemyAttackDamageSniper);
	DOREPLIFETIME(APlayerCharacter, RandEnemyAttackDamageM249);
}

void APlayerCharacter::Fire()
{
	// 사격 가능 상태가 아니거나, 뛰고 있거나, 위젯이 켜져 있거나, 엔딩 연출 중이라면 리턴
	if (!CanShoot || isRunning || gi->IsWidgetOn || bEnding || IsPlayerDeadImmediately || UGameplayStatics::GetCurrentLevelName(GetWorld()) == FString("Safe_House"))
	{
		return;
	}
	if (!isZooming && weaponArray[1] == false && weaponArray[2]==false)
	{
		GetWorldTimerManager().ClearTimer(ZoomFireHandle);
		Zoom();
	}
	ServerRPCFire();
	CanShoot = false;
	GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		CanShoot = true;
	}), 1 / (SetFireInterval() * FireRateMultiplier()), false);
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

			// 서버 로직 (핵심 프로세스 처리)
			if (HasAuthority())
			{
				ProcessRifleFire();
			}
			// 실행하는 주체 (서버 / 클라 무관, 자신에게만 실행되는 로직 구현)
			if (IsLocallyControlled())
			{
				ProcessRifleFireLocal();
			}
			// Simulated Proxy
			else
			{
				ProcessRifleFireSimulatedProxy();
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

			// 서버 로직 (핵심 프로세스 처리)
			if (HasAuthority())
			{
				ProcessSniperFire();
			}
			// 실행하는 주체 (서버 / 클라 무관, 자신에게만 실행되는 로직 구현)
			if (IsLocallyControlled())
			{
				ProcessSniperFireLocal();
			}
			// Simulated Proxy
			else
			{
				ProcessSniperFireSimulatedProxy();
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

			// 서버 로직 (핵심 프로세스 처리)
			if (HasAuthority())
			{
				ProcessPistolFire();
			}
			// 실행하는 주체 (서버 / 클라 무관, 자신에게만 실행되는 로직 구현)
			if (IsLocallyControlled())
			{
				ProcessPistolFireLocal();
			}
			// Simulated Proxy
			else
			{
				ProcessPistolFireSimulatedProxy();
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

			// 서버 로직 (핵심 프로세스 처리)
			if (HasAuthority())
			{
				ProcessM249Fire();
			}
			// 실행하는 주체 (서버 / 클라 무관, 자신에게만 실행되는 로직 구현)
			if (IsLocallyControlled())
			{
				ProcessM249FireLocal();
			}
			// Simulated Proxy
			else
			{
				ProcessM249FireSimulatedProxy();
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

void APlayerCharacter::ExtractionSuccess()
{
	UE_LOG(LogTemp, Warning, TEXT("Extraction Success"))
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PlayerSpawnEmitter, GetActorLocation());
	UGameplayStatics::PlaySound2D(GetWorld(), ExtractionSound);
	APlayerCameraManager* playerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	// 카메라 페이드 연출
	playerCam->StartCameraFade(0, 1, 1.5f, FLinearColor::Black, false, true);
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
	const FVector particleLoc = rifleComp->GetSocketLocation(FName("RifleFirePosition"));
	const UE::Math::TRotator<double> particleRot = rifleComp->GetSocketRotation(FName("RifleFirePosition"));
	const FTransform particleTrans = UKismetMathLibrary::MakeTransform(particleLoc, particleRot, FVector(0.4));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RifleFireParticle2, particleTrans);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), RifleFireSound, GetActorLocation());

	const double randF = UKismetMathLibrary::RandomFloatInRange(-0.3 * RecoilRateMultiplier(), -0.5 * RecoilRateMultiplier());
	const double randF2 = UKismetMathLibrary::RandomFloatInRange(-0.3 * RecoilRateMultiplier(), 0.3 * RecoilRateMultiplier());
	AddControllerPitchInput(randF);
	AddControllerYawInput(randF2);
}

void APlayerCharacter::ProcessRifleFireSimulatedProxy()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), RifleFireSound, GetActorLocation());
	const FVector particleLoc = rifleComp->GetSocketLocation(FName("RifleFirePosition"));
	const UE::Math::TRotator<double> particleRot = rifleComp->GetSocketRotation(FName("RifleFirePosition"));
	const FTransform particleTrans = UKismetMathLibrary::MakeTransform(particleLoc, particleRot, FVector(0.4));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RifleFireParticle2, particleTrans);
}

void APlayerCharacter::FireRelease()
{
	if (!IsZoomKeyPressed && weaponArray[1] == false && weaponArray[2]==false)
	{
		GetWorld()->GetTimerManager().SetTimer(ZoomFireHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			ZoomRelease();
		}), 0.5f, false);
	}
	EmptySoundBoolean = false;
}

void APlayerCharacter::ProcessRifleFire()
{
	if (curRifleAmmo > 0)
	{
		// Clamp를 통한 탄약 수 차감
		curRifleAmmo = FMath::Clamp(curRifleAmmo - 1, 0, 40 + SetRifleAdditionalMagazine());
		FHitResult rifleHitResult;
		FVector startLoc = FollowCamera->GetComponentLocation();
		FVector EndLoc = startLoc + FollowCamera->GetForwardVector() * 10000.0f;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		// Perform Linetrace
		bool bHit = GetWorld()->LineTraceSingleByChannel(rifleHitResult, startLoc, EndLoc, ECC_Visibility, params);
		if (bHit)
		{
			// Player Character Casting
			APlayerCharacter* player = Cast<APlayerCharacter>(rifleHitResult.GetActor());
			// 플레이어 적중
			if (player)
			{
				if (rifleHitResult.BoneName == FName("head"))
				{
					OnPlayerHit(rifleHitResult, player, true);
				}
				else
				{
					OnPlayerHit(rifleHitResult, player, false);
				}
				return;
			}
			// Enemy Casting
			AEnemy* enemy = Cast<AEnemy>(rifleHitResult.GetActor());
			if (enemy)
			{
				if (rifleHitResult.BoneName == FName("head"))
				{
					OnEnemyHit(rifleHitResult, enemy, true);
				}
				else
				{
					OnEnemyHit(rifleHitResult, enemy, false);
				}
				return;
			}
			// Reward Container Casting
			ARewardContainer* rewardContainer = Cast<ARewardContainer>(rifleHitResult.GetActor());
			if (rewardContainer)
			{
				OnContainerHit(rifleHitResult, rewardContainer);
			}
			// 지형지물에 적중
			else
			{
				OnGroundHit(rifleHitResult);
			}
		}
	}
}


void APlayerCharacter::ProcessSniperFire()
{
	if (curSniperAmmo > 0)
	{
		// Clamp를 통한 탄약 수 차감
		curSniperAmmo = FMath::Clamp(curSniperAmmo - 1, 0, 5 + SetSniperAdditionalMagazine());
		FHitResult sniperHitResult;
		FVector startLoc = FollowCamera->GetComponentLocation();
		FVector EndLoc = startLoc + FollowCamera->GetForwardVector() * 10000.0f;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		// Perform Linetrace
		bool bHit = GetWorld()->LineTraceSingleByChannel(sniperHitResult, startLoc, EndLoc, ECC_Visibility, params);
		// 라인 트레이스가 적중했다면
		if (bHit)
		{
			// Player Character Casting
			APlayerCharacter* player = Cast<APlayerCharacter>(sniperHitResult.GetActor());
			// 플레이어 적중
			if (player)
			{
				if (sniperHitResult.BoneName == FName("head"))
				{
					OnPlayerHit(sniperHitResult, player, true);
				}
				else
				{
					OnPlayerHit(sniperHitResult, player, false);
				}
				return;
			}
			// Enemy Casting
			AEnemy* enemy = Cast<AEnemy>(sniperHitResult.GetActor());
			if (enemy)
			{
				if (sniperHitResult.BoneName == FName("head"))
				{
					OnEnemyHit(sniperHitResult, enemy, true);
				}
				else
				{
					OnEnemyHit(sniperHitResult, enemy, false);
				}
				return;
			}
			// // Enemy Casting
			// AEnemy* enemy = Cast<AEnemy>(sniperHitResult.GetActor());
			// // Enemy FSM Casting
			// UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
			// // Reward Container Casting
			// ARewardContainer* rewardContainer = Cast<ARewardContainer>(sniperHitResult.GetActor());
			// if (fsm && enemy)
			// {
			// 	AGuardian* guardian = Cast<AGuardian>(enemy);
			// 	ACrunch* crunch = Cast<ACrunch>(enemy);
			// 	if (guardian)
			// 	{
			// 		bGuardian = true;
			// 	}
			// 	else if (crunch)
			// 	{
			// 		bCrunch = true;
			// 	}
			// 	// 이미 죽지 않은 적에게만 실행
			// 	if (enemy->bDeath == false)
			// 	{
			// 		hitActors = sniperHitResult.GetActor();
			// 		FName hitBone = sniperHitResult.BoneName;
			// 		FVector_NetQuantize hitLoc = sniperHitResult.Location;
			// 		FRotator hitRot = UKismetMathLibrary::Conv_VectorToRotator(sniperHitResult.ImpactNormal);
			// 		if (hitBone == FName("head"))
			// 		{
			// 			float min = FMath::RoundFromZero(1000 * DamageMultiplier());
			// 			float max = FMath::RoundFromZero(1200 * DamageMultiplier());
			// 			randSniperHeadDamage = FMath::RandRange(min, max);
			// 			if (enemy->EnemyStat->GetCurrentHp() <= randSniperHeadDamage)
			// 			{
			// 				crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
			// 				UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
			// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(3.0f));
			// 				// FSM에 있는 Damage Process 호출		
			// 				fsm->OnDamageProcess(randSniperHeadDamage);
			// 				SetDamageWidget(randSniperHeadDamage, hitLoc, false, FLinearColor::Yellow);
			// 				// 헤드 적중 데미지 프로세스 호출
			// 				enemy->OnHeadDamaged();
			// 				enemy->bDeath = true;
			// 				if (bGuardian)
			// 				{
			// 					GuardianCount++;
			// 					informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
			// 					enemy->DropReward();
			// 				}
			// 				else if (bCrunch)
			// 				{
			// 					BossCount++;
			// 					informationUI->BossCount->SetText(FText::AsNumber(BossCount));
			// 					SetBossHPWidget(enemy);
			// 					enemy->DropReward();
			// 					FTimerHandle removeHandle;
			// 					GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
			// 				}
			// 			}
			// 			else
			// 			{
			// 				crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
			// 				UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHeadHitSound, hitLoc);
			// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(3.0f));
			// 				// 적중 대상이 보스라면
			// 				if (bCrunch)
			// 				{
			// 					// 실드가 파괴된 상태라면
			// 					if (enemy->EnemyStat->isShieldBroken)
			// 					{
			// 						// FSM에 있는 Damage Process 호출		
			// 						fsm->OnDamageProcess(randSniperHeadDamage);
			// 						if (enemy->isStunned)
			// 						{
			// 							// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 							SetDamageWidget(randSniperHeadDamage * 2, hitLoc, false, FLinearColor::Red);
			// 						}
			// 						else
			// 						{
			// 							// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 							SetDamageWidget(randSniperHeadDamage, hitLoc, false, FLinearColor::Yellow);
			// 						}
			// 						// 헤드 적중 데미지 프로세스 호출
			// 						enemy->OnHeadDamaged();
			// 					}
			// 					// 실드가 파괴되지 않은 상태라면
			// 					else
			// 					{
			// 						FTransform EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
			// 						EmitterTrans.SetScale3D(FVector(1.3));
			// 						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
			// 						// FSM에 있는 Damage Process 호출		
			// 						fsm->OnShieldDamageProcess(randSniperHeadDamage);
			// 						// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 						SetDamageWidget(randSniperHeadDamage / 20, hitLoc, true, FLinearColor::Yellow);
			// 						// 헤드 적중 데미지 프로세스 호출
			// 						enemy->OnHeadDamaged();
			// 					}
			// 					SetBossHPWidget(enemy);
			// 				}
			// 				// 보스가 아니라면
			// 				else
			// 				{
			// 					// FSM에 있는 Damage Process 호출		
			// 					fsm->OnDamageProcess(randSniperHeadDamage);
			// 					// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 					SetDamageWidget(randSniperHeadDamage, hitLoc, false, FLinearColor::Yellow);
			// 					// 헤드 적중 데미지 프로세스 호출
			// 					enemy->OnHeadDamaged();
			// 				}
			// 			}
			// 		}
			// 		else
			// 		{
			// 			float min = FMath::RoundFromZero(650 * DamageMultiplier());
			// 			float max = FMath::RoundFromZero(850 * DamageMultiplier());
			// 			randSniperDamage = FMath::RandRange(min, max);
			// 			if (enemy->EnemyStat->GetCurrentHp() <= randSniperDamage)
			// 			{
			// 				crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
			// 				UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
			// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(3.0f));
			// 				// FSM에 있는 Damage Process 호출		
			// 				fsm->OnDamageProcess(randSniperDamage);
			// 				SetDamageWidget(randSniperDamage, hitLoc, false, FLinearColor::White);
			//
			// 				// 일반 적중 데미지 프로세스 호출
			// 				enemy->OnDamaged();
			// 				enemy->bDeath = true;
			// 				if (bGuardian)
			// 				{
			// 					GuardianCount++;
			// 					informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
			// 					enemy->DropReward();
			// 				}
			// 				else if (bCrunch)
			// 				{
			// 					BossCount++;
			// 					informationUI->BossCount->SetText(FText::AsNumber(BossCount));
			// 					SetBossHPWidget(enemy);
			// 					enemy->DropReward();
			// 					FTimerHandle removeHandle;
			// 					GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
			// 				}
			// 			}
			// 			else
			// 			{
			// 				crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
			// 				UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, hitLoc);
			// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(0.8f));
			// 				// 적중 대상이 보스라면
			// 				if (bCrunch)
			// 				{
			// 					// 실드가 파괴된 상태라면
			// 					if (enemy->EnemyStat->isShieldBroken)
			// 					{
			// 						// FSM에 있는 Damage Process 호출		
			// 						fsm->OnDamageProcess(randSniperDamage);
			// 						if (enemy->isStunned)
			// 						{
			// 							// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 							SetDamageWidget(randSniperDamage * 2, hitLoc, false, FLinearColor::Red);
			// 						}
			// 						else
			// 						{
			// 							// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 							SetDamageWidget(randSniperDamage, hitLoc, false, FLinearColor::White);
			// 						}
			// 						// 일반 적중 데미지 프로세스 호출
			// 						enemy->OnDamaged();
			// 					}
			// 					// 실드가 있는 상태라면
			// 					else
			// 					{
			// 						FTransform EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
			// 						EmitterTrans.SetScale3D(FVector(1.3));
			// 						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
			// 						// FSM에 있는 Damage Process 호출		
			// 						fsm->OnShieldDamageProcess(randSniperDamage);
			// 						// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 						SetDamageWidget(randSniperDamage / 20, hitLoc, true, FLinearColor::White);
			// 						// 일반 적중 데미지 프로세스 호출
			// 						enemy->OnDamaged();
			// 					}
			// 					SetBossHPWidget(enemy);
			// 				}
			// 				// 보스가 아니라면
			// 				else
			// 				{
			// 					// FSM에 있는 Damage Process 호출		
			// 					fsm->OnDamageProcess(randSniperDamage);
			// 					// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 					SetDamageWidget(randSniperDamage, hitLoc, false, FLinearColor::White);
			// 					// 일반 적중 데미지 프로세스 호출
			// 					enemy->OnDamaged();
			// 				}
			// 			}
			// 		}
			// 	}
			// 	bGuardian = false;
			// 	bCrunch = false;
			// }
			// Reward Container Casting
			ARewardContainer* rewardContainer = Cast<ARewardContainer>(sniperHitResult.GetActor());
			if (rewardContainer)
			{
				OnContainerHit(sniperHitResult, rewardContainer);
			}
			else
			{
				OnGroundHit(sniperHitResult);
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
	const double randF = UKismetMathLibrary::RandomFloatInRange(-0.7 * RecoilRateMultiplier(), -1.2 * RecoilRateMultiplier());
	const double randF2 = UKismetMathLibrary::RandomFloatInRange(-0.7 * RecoilRateMultiplier(), 0.8 * RecoilRateMultiplier());
	AddControllerPitchInput(randF);
	AddControllerYawInput(randF2);
	if (isZooming)
	{
		const UE::Math::TVector<double> particleTrans = FollowCamera->GetComponentLocation() + FollowCamera->GetUpVector() * -70.0f;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SniperFireParticle, particleTrans);
		PC->PlayerCameraManager->StartCameraShake(sniperCameraShake);
	}
	else
	{
		FTransform particleTrans = sniperComp->GetSocketTransform(FName("SniperFirePosition"));
		particleTrans.SetScale3D(FVector(0.7));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SniperFireParticle, particleTrans);
		PC->PlayerCameraManager->StartCameraShake(sniperFireShake);
	}
}

void APlayerCharacter::ProcessSniperFireSimulatedProxy()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SniperFireSound, GetActorLocation());
	FTransform particleTrans = sniperComp->GetSocketTransform(FName("SniperFirePosition"));
	particleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SniperFireParticle, particleTrans);
}

void APlayerCharacter::ProcessPistolFire()
{
	if (curPistolAmmo > 0)
	{
		// Clamp를 통한 탄약 수 차감
		curPistolAmmo = FMath::Clamp(curPistolAmmo - 1, 0, 8 + SetPistolAdditionalMagazine());
		FHitResult pistolHitResult;
		FVector startLoc = FollowCamera->GetComponentLocation();
		FVector EndLoc = startLoc + FollowCamera->GetForwardVector() * 10000.0f;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		// Perform Linetrace
		bool bHit = GetWorld()->LineTraceSingleByChannel(pistolHitResult, startLoc, EndLoc, ECC_Visibility, params);
		if (bHit)
		{
			// Player Character Casting
			APlayerCharacter* player = Cast<APlayerCharacter>(pistolHitResult.GetActor());
			// 플레이어 적중
			if (player)
			{
				if (pistolHitResult.BoneName == FName("head"))
				{
					OnPlayerHit(pistolHitResult, player, true);
				}
				else
				{
					OnPlayerHit(pistolHitResult, player, false);
				}
				return;
			}
			// Enemy Casting
			AEnemy* enemy = Cast<AEnemy>(pistolHitResult.GetActor());
			if (enemy)
			{
				if (pistolHitResult.BoneName == FName("head"))
				{
					OnEnemyHit(pistolHitResult, enemy, true);
				}
				else
				{
					OnEnemyHit(pistolHitResult, enemy, false);
				}
				return;
			}
			// // Enemy Casting
			// AEnemy* enemy = Cast<AEnemy>(pistolHitResult.GetActor());
			// // Enemy FSM Casting
			// UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
			// // Reward Container Casting
			// ARewardContainer* rewardContainer = Cast<ARewardContainer>(pistolHitResult.GetActor());
			// if (fsm && enemy)
			// {
			// 	AGuardian* guardian = Cast<AGuardian>(enemy);
			// 	ACrunch* crunch = Cast<ACrunch>(enemy);
			// 	if (guardian)
			// 	{
			// 		bGuardian = true;
			// 	}
			// 	else if (crunch)
			// 	{
			// 		bCrunch = true;
			// 	}
			// 	// 이미 죽지 않은 적에게만 실행
			// 	if (enemy->bDeath == false)
			// 	{
			// 		hitActors = pistolHitResult.GetActor();
			// 		FName hitBone = pistolHitResult.BoneName;
			// 		FVector_NetQuantize hitLoc = pistolHitResult.Location;
			// 		FRotator hitRot = UKismetMathLibrary::Conv_VectorToRotator(pistolHitResult.ImpactNormal);
			// 		if (hitBone == FName("head"))
			// 		{
			// 			float min = FMath::RoundFromZero(450 * DamageMultiplier());
			// 			float max = FMath::RoundFromZero(550 * DamageMultiplier());
			// 			randPistolHeadDamage = FMath::RandRange(min, max);
			// 			// 이번 공격에 Enemy가 죽는다면
			// 			if (enemy->EnemyStat->GetCurrentHp() <= randPistolHeadDamage)
			// 			{
			// 				crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
			// 				UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
			// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.5f));
			// 				// FSM에 있는 Damage Process 호출		
			// 				fsm->OnDamageProcess(randPistolHeadDamage);
			// 				SetDamageWidget(randPistolHeadDamage, hitLoc, false, FLinearColor::Yellow);
			//
			// 				// 헤드 적중 데미지 프로세스 호출
			// 				enemy->OnHeadDamaged();
			// 				enemy->bDeath = true;
			// 				if (bGuardian)
			// 				{
			// 					GuardianCount++;
			// 					informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
			// 					enemy->DropReward();
			// 				}
			// 				else if (bCrunch)
			// 				{
			// 					BossCount++;
			// 					informationUI->BossCount->SetText(FText::AsNumber(BossCount));
			// 					SetBossHPWidget(enemy);
			// 					enemy->DropReward();
			// 					FTimerHandle removeHandle;
			// 					GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
			// 				}
			// 			}
			// 			else
			// 			{
			// 				crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
			// 				UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHeadHitSound, hitLoc);
			// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.5f));
			// 				// 적중 대상이 보스라면
			// 				if (bCrunch)
			// 				{
			// 					// 실드가 파괴된 상태라면
			// 					if (enemy->EnemyStat->isShieldBroken)
			// 					{
			// 						// FSM에 있는 Damage Process 호출		
			// 						fsm->OnDamageProcess(randPistolHeadDamage);
			// 						if (enemy->isStunned)
			// 						{
			// 							// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 							SetDamageWidget(randPistolHeadDamage * 2, hitLoc, false, FLinearColor::Red);
			// 						}
			// 						else
			// 						{
			// 							// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 							SetDamageWidget(randPistolHeadDamage, hitLoc, false, FLinearColor::Yellow);
			// 						}
			// 						// 헤드 적중 데미지 프로세스 호출
			// 						enemy->OnHeadDamaged();
			// 					}
			// 					// 실드가 파괴되지 않은 상태라면
			// 					else
			// 					{
			// 						FTransform EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
			// 						EmitterTrans.SetScale3D(FVector(1.3));
			// 						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
			// 						// FSM에 있는 Damage Process 호출		
			// 						fsm->OnShieldDamageProcess(randPistolHeadDamage);
			// 						// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 						SetDamageWidget(randPistolHeadDamage / 20, hitLoc, true, FLinearColor::Yellow);
			// 						// 헤드 적중 데미지 프로세스 호출
			// 						enemy->OnHeadDamaged();
			// 					}
			// 					SetBossHPWidget(enemy);
			// 				}
			// 				// 보스가 아니라면
			// 				else
			// 				{
			// 					// FSM에 있는 Damage Process 호출		
			// 					fsm->OnDamageProcess(randPistolHeadDamage);
			// 					// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 					SetDamageWidget(randPistolHeadDamage, hitLoc, false, FLinearColor::Yellow);
			// 					// 헤드 적중 데미지 프로세스 호출
			// 					enemy->OnHeadDamaged();
			// 				}
			// 			}
			// 		}
			// 		else
			// 		{
			// 			float min = FMath::RoundFromZero(220 * DamageMultiplier());
			// 			float max = FMath::RoundFromZero(300 * DamageMultiplier());
			// 			randPistolDamage = FMath::RandRange(min, max);
			// 			// 이번 공격에 Enemy가 죽는다면
			// 			if (enemy->EnemyStat->GetCurrentHp() <= randPistolDamage)
			// 			{
			// 				crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
			// 				UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
			// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.5f));
			// 				// FSM에 있는 Damage Process 호출		
			// 				fsm->OnDamageProcess(randPistolDamage);
			// 				SetDamageWidget(randPistolDamage, hitLoc, false, FLinearColor::White);
			//
			// 				// 일반 적중 데미지 프로세스 호출
			// 				enemy->OnDamaged();
			// 				enemy->bDeath = true;
			// 				if (bGuardian)
			// 				{
			// 					GuardianCount++;
			// 					informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
			// 					enemy->DropReward();
			// 				}
			// 				else if (bCrunch)
			// 				{
			// 					BossCount++;
			// 					informationUI->BossCount->SetText(FText::AsNumber(BossCount));
			// 					SetBossHPWidget(enemy);
			// 					enemy->DropReward();
			// 					FTimerHandle removeHandle;
			// 					GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
			// 				}
			// 			}
			// 			else
			// 			{
			// 				crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
			// 				UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, hitLoc);
			// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(0.7f));
			// 				// 적중 대상이 보스라면
			// 				if (bCrunch)
			// 				{
			// 					// 실드가 파괴된 상태라면
			// 					if (enemy->EnemyStat->isShieldBroken)
			// 					{
			// 						// FSM에 있는 Damage Process 호출		
			// 						fsm->OnDamageProcess(randPistolDamage);
			// 						if (enemy->isStunned)
			// 						{
			// 							// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 							SetDamageWidget(randPistolDamage * 2, hitLoc, false, FLinearColor::Red);
			// 						}
			// 						else
			// 						{
			// 							// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 							SetDamageWidget(randPistolDamage, hitLoc, false, FLinearColor::White);
			// 						}
			// 						// 일반 적중 데미지 프로세스 호출
			// 						enemy->OnDamaged();
			// 					}
			// 					// 실드가 있는 상태라면
			// 					else
			// 					{
			// 						FTransform EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
			// 						EmitterTrans.SetScale3D(FVector(1.3));
			// 						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
			// 						// FSM에 있는 Damage Process 호출		
			// 						fsm->OnShieldDamageProcess(randPistolDamage);
			// 						// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 						SetDamageWidget(randPistolDamage / 20, hitLoc, true, FLinearColor::White);
			// 						// 일반 적중 데미지 프로세스 호출
			// 						enemy->OnDamaged();
			// 					}
			// 					SetBossHPWidget(enemy);
			// 				}
			// 				// 보스가 아니라면
			// 				else
			// 				{
			// 					// FSM에 있는 Damage Process 호출		
			// 					fsm->OnDamageProcess(randPistolDamage);
			// 					// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 					SetDamageWidget(randPistolDamage, hitLoc, false, FLinearColor::White);
			// 					// 일반 적중 데미지 프로세스 호출
			// 					enemy->OnDamaged();
			// 				}
			// 			}
			// 		}
			// 	}
			// 	bGuardian = false;
			// 	bCrunch = false;
			// }
			// Reward Container Casting
			ARewardContainer* rewardContainer = Cast<ARewardContainer>(pistolHitResult.GetActor());
			if (rewardContainer)
			{
				OnContainerHit(pistolHitResult, rewardContainer);
			}
			else
			{
				OnGroundHit(pistolHitResult);
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
	FTransform particleTrans = pistolComp->GetSocketTransform(FName("PistolFirePosition"));
	particleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolfireParticle, particleTrans);

	double randF = UKismetMathLibrary::RandomFloatInRange(-0.7 * RecoilRateMultiplier(), -1.2 * RecoilRateMultiplier());
	double randF2 = UKismetMathLibrary::RandomFloatInRange(-0.7 * RecoilRateMultiplier(), 0.8 * RecoilRateMultiplier());
	AddControllerPitchInput(randF);
	AddControllerYawInput(randF2);
}

void APlayerCharacter::ProcessPistolFireSimulatedProxy()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PistolFireSound, GetActorLocation());
	FTransform particleTrans = pistolComp->GetSocketTransform(FName("PistolFirePosition"));
	particleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolfireParticle, particleTrans);
}

void APlayerCharacter::ProcessM249Fire()
{
	if (curM249Ammo > 0)
	{
		// Clamp를 통한 탄약 수 차감
		curM249Ammo = FMath::Clamp(curM249Ammo - 1, 0, 100 + SetM249AdditionalMagazine());
		FHitResult M249HitResult;
		FVector startLoc = FollowCamera->GetComponentLocation();
		FVector EndLoc = startLoc + FollowCamera->GetForwardVector() * 10000.0f;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		// Perform Linetrace
		bool bHit = GetWorld()->LineTraceSingleByChannel(M249HitResult, startLoc, EndLoc, ECC_Visibility, params);
		if (bHit)
		{
			// Player Character Casting
			APlayerCharacter* player = Cast<APlayerCharacter>(M249HitResult.GetActor());
			// 플레이어 적중
			if (player)
			{
				if (M249HitResult.BoneName == FName("head"))
				{
					OnPlayerHit(M249HitResult, player, true);
				}
				else
				{
					OnPlayerHit(M249HitResult, player, false);
				}
				return;
			}
			// Enemy Casting
			AEnemy* enemy = Cast<AEnemy>(M249HitResult.GetActor());
			if (enemy)
			{
				if (M249HitResult.BoneName == FName("head"))
				{
					OnEnemyHit(M249HitResult, enemy, true);
				}
				else
				{
					OnEnemyHit(M249HitResult, enemy, false);
				}
				return;
			}
			// // Enemy Casting
			// AEnemy* enemy = Cast<AEnemy>(M249HitResult.GetActor());
			// // Enemy FSM Casting
			// UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
			// // Reward Container Casting
			// ARewardContainer* rewardContainer = Cast<ARewardContainer>(M249HitResult.GetActor());
			// if (fsm && enemy)
			// {
			// 	AGuardian* guardian = Cast<AGuardian>(enemy);
			// 	ACrunch* crunch = Cast<ACrunch>(enemy);
			// 	if (guardian)
			// 	{
			// 		bGuardian = true;
			// 	}
			// 	else if (crunch)
			// 	{
			// 		bCrunch = true;
			// 	}
			// 	// 이미 죽지 않은 적에게만 실행
			// 	if (enemy->bDeath == false)
			// 	{
			// 		hitActors = M249HitResult.GetActor();
			// 		FName hitBone = M249HitResult.BoneName;
			// 		FVector_NetQuantize hitLoc = M249HitResult.Location;
			// 		FRotator hitRot = UKismetMathLibrary::Conv_VectorToRotator(M249HitResult.ImpactNormal);
			// 		if (hitBone == FName("head"))
			// 		{
			// 			float min = FMath::RoundFromZero(180 * DamageMultiplier());
			// 			float max = FMath::RoundFromZero(220 * DamageMultiplier());
			// 			randM249HeadDamage = FMath::RandRange(min, max);
			// 			// 이번 공격에 Enemy가 죽는다면
			// 			if (enemy->EnemyStat->GetCurrentHp() <= randM249HeadDamage)
			// 			{
			// 				crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
			// 				UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
			// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.0f));
			// 				// FSM에 있는 Damage Process 호출		
			// 				fsm->OnDamageProcess(randM249HeadDamage);
			// 				SetDamageWidget(randM249HeadDamage, hitLoc, false, FLinearColor::Yellow);
			//
			// 				// 헤드 적중 데미지 프로세스 호출
			// 				enemy->OnHeadDamaged();
			// 				enemy->bDeath = true;
			// 				if (bGuardian)
			// 				{
			// 					GuardianCount++;
			// 					enemy->DropReward();
			// 					informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
			// 				}
			// 				else if (bCrunch)
			// 				{
			// 					BossCount++;
			// 					informationUI->BossCount->SetText(FText::AsNumber(BossCount));
			// 					SetBossHPWidget(enemy);
			// 					enemy->DropReward();
			// 					FTimerHandle removeHandle;
			// 					GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
			// 				}
			// 			}
			// 			else
			// 			{
			// 				crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
			// 				UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHeadHitSound, hitLoc);
			// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.0f));
			// 				// 적중 대상이 보스라면
			// 				if (bCrunch)
			// 				{
			// 					// 실드가 파괴된 상태라면
			// 					if (enemy->EnemyStat->isShieldBroken)
			// 					{
			// 						// FSM에 있는 Damage Process 호출		
			// 						fsm->OnDamageProcess(randM249HeadDamage);
			// 						if (enemy->isStunned)
			// 						{
			// 							// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 							SetDamageWidget(randM249HeadDamage * 2, hitLoc, false, FLinearColor::Red);
			// 						}
			// 						else
			// 						{
			// 							// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 							SetDamageWidget(randM249HeadDamage, hitLoc, false, FLinearColor::Yellow);
			// 						}
			// 						// 헤드 적중 데미지 프로세스 호출
			// 						enemy->OnHeadDamaged();
			// 					}
			// 					// 실드가 파괴되지 않은 상태라면
			// 					else
			// 					{
			// 						FTransform EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
			// 						EmitterTrans.SetScale3D(FVector(1.3));
			// 						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
			// 						// FSM에 있는 Damage Process 호출		
			// 						fsm->OnShieldDamageProcess(randM249HeadDamage);
			// 						// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 						SetDamageWidget(randM249HeadDamage / 20, hitLoc, true, FLinearColor::Yellow);
			// 						// 헤드 적중 데미지 프로세스 호출
			// 						enemy->OnHeadDamaged();
			// 					}
			// 					SetBossHPWidget(enemy);
			// 				}
			// 				// 보스가 아니라면
			// 				else
			// 				{
			// 					// FSM에 있는 Damage Process 호출		
			// 					fsm->OnDamageProcess(randM249HeadDamage);
			// 					// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 					SetDamageWidget(randM249HeadDamage, hitLoc, false, FLinearColor::Yellow);
			// 					// 헤드 적중 데미지 프로세스 호출
			// 					enemy->OnHeadDamaged();
			// 				}
			// 			}
			// 		}
			// 		else
			// 		{
			// 			float min = FMath::RoundFromZero(90 * DamageMultiplier());
			// 			float max = FMath::RoundFromZero(110 * DamageMultiplier());
			// 			randM249Damage = FMath::RandRange(min, max);
			// 			// 이번 공격에 Enemy가 죽는다면
			// 			if (enemy->EnemyStat->GetCurrentHp() <= randM249Damage)
			// 			{
			// 				crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
			// 				UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
			// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.0f));
			// 				// FSM에 있는 Damage Process 호출		
			// 				fsm->OnDamageProcess(randM249Damage);
			// 				SetDamageWidget(randM249Damage, hitLoc, false, FLinearColor::White);
			//
			// 				// 일반 적중 데미지 프로세스 호출
			// 				enemy->OnDamaged();
			// 				enemy->bDeath = true;
			// 				if (bGuardian)
			// 				{
			// 					GuardianCount++;
			// 					enemy->DropReward();
			// 					informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
			// 				}
			// 				else if (bCrunch)
			// 				{
			// 					BossCount++;
			// 					informationUI->BossCount->SetText(FText::AsNumber(BossCount));
			// 					SetBossHPWidget(enemy);
			// 					enemy->DropReward();
			// 					FTimerHandle removeHandle;
			// 					GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
			// 				}
			// 			}
			// 			else
			// 			{
			// 				crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
			// 				UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, hitLoc);
			// 				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(0.5f));
			// 				// 적중 대상이 보스라면
			// 				if (bCrunch)
			// 				{
			// 					// 실드가 파괴된 상태라면
			// 					if (enemy->EnemyStat->isShieldBroken)
			// 					{
			// 						// FSM에 있는 Damage Process 호출		
			// 						fsm->OnDamageProcess(randM249Damage);
			// 						if (enemy->isStunned)
			// 						{
			// 							// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 							SetDamageWidget(randM249Damage * 2, hitLoc, false, FLinearColor::Red);
			// 						}
			// 						else
			// 						{
			// 							// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 							SetDamageWidget(randM249Damage, hitLoc, false, FLinearColor::White);
			// 						}
			// 						// 일반 적중 데미지 프로세스 호출
			// 						enemy->OnDamaged();
			// 					}
			// 					// 실드가 있는 상태라면
			// 					else
			// 					{
			// 						FTransform EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
			// 						EmitterTrans.SetScale3D(FVector(1.3));
			// 						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
			// 						// FSM에 있는 Damage Process 호출		
			// 						fsm->OnShieldDamageProcess(randM249Damage);
			// 						// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 						SetDamageWidget(randM249Damage / 20, hitLoc, true, FLinearColor::White);
			// 						// 일반 적중 데미지 프로세스 호출
			// 						enemy->OnDamaged();
			// 					}
			// 					SetBossHPWidget(enemy);
			// 				}
			// 				// 보스가 아니라면
			// 				else
			// 				{
			// 					// FSM에 있는 Damage Process 호출		
			// 					fsm->OnDamageProcess(randM249Damage);
			// 					// 데미지 위젯에 피해 값과 적 위치벡터 할당
			// 					SetDamageWidget(randM249Damage, hitLoc, false, FLinearColor::White);
			// 					// 일반 적중 데미지 프로세스 호출
			// 					enemy->OnDamaged();
			// 				}
			// 			}
			// 		}
			// 	}
			// 	bGuardian = false;
			// 	bCrunch = false;
			// }
			// Reward Container Casting
			ARewardContainer* rewardContainer = Cast<ARewardContainer>(M249HitResult.GetActor());
			if (rewardContainer)
			{
				OnContainerHit(M249HitResult, rewardContainer);
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
	FTransform particleTrans = m249Comp->GetSocketTransform(FName("M249FirePosition"));
	particleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolfireParticle, particleTrans);
	UGameplayStatics::PlaySound2D(GetWorld(), M249FireSound);
	PC->PlayerCameraManager->StartCameraShake(rifleFireShake);
	if (isZooming)
	{
		const double randF = UKismetMathLibrary::RandomFloatInRange(-0.4 * RecoilRateMultiplier(), -0.7 * RecoilRateMultiplier());
		const double randF2 = UKismetMathLibrary::RandomFloatInRange(-0.4 * RecoilRateMultiplier(), 0.4 * RecoilRateMultiplier());
		AddControllerPitchInput(randF);
		AddControllerYawInput(randF2);
	}
	else
	{
		const double randF = UKismetMathLibrary::RandomFloatInRange(-0.6 * RecoilRateMultiplier(), -1.1 * RecoilRateMultiplier());
		const double randF2 = UKismetMathLibrary::RandomFloatInRange(-0.5 * RecoilRateMultiplier(), 0.5 * RecoilRateMultiplier());
		AddControllerPitchInput(randF);
		AddControllerYawInput(randF2);
	}
}

void APlayerCharacter::ProcessM249FireSimulatedProxy()
{
	FTransform particleTrans = m249Comp->GetSocketTransform(FName("M249FirePosition"));
	particleTrans.SetScale3D(FVector(0.7));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolfireParticle, particleTrans);
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

float APlayerCharacter::GetAttackDamage(bool IsPlayer)
{
	if (weaponArray[0] == true)
	{
		if (IsPlayer)
		{
			RandPlayerAttackDamageRifle = GenerateRandomDamage(PlayerAttackDamageRifle);
			return RandPlayerAttackDamageRifle;
		}
		RandEnemyAttackDamageRifle = GenerateRandomDamage(EnemyAttackDamageRifle);
		return RandEnemyAttackDamageRifle;
	}
	if (weaponArray[1] == true)
	{
		if (IsPlayer)
		{
			RandPlayerAttackDamageSniper = GenerateRandomDamage(PlayerAttackDamageSniper);
			return RandPlayerAttackDamageSniper;
		}
		RandEnemyAttackDamageSniper = GenerateRandomDamage(EnemyAttackDamageSniper);
		return RandEnemyAttackDamageSniper;
	}
	if (weaponArray[2] == true)
	{
		if (IsPlayer)
		{
			RandPlayerAttackDamagePistol = GenerateRandomDamage(PlayerAttackDamagePistol);
			return RandPlayerAttackDamagePistol;
		}
		RandEnemyAttackDamagePistol = GenerateRandomDamage(EnemyAttackDamagePistol);
		return RandEnemyAttackDamagePistol;
	}
	if (weaponArray[3] == true)
	{
		if (IsPlayer)
		{
			RandPlayerAttackDamageM249 = GenerateRandomDamage(PlayerAttackDamageM249);
			return RandPlayerAttackDamageM249;
		}
		RandEnemyAttackDamageM249 = GenerateRandomDamage(EnemyAttackDamageM249);
		return RandEnemyAttackDamageM249;
	}
	return 0;
}

int32 APlayerCharacter::GetAttackDamageCache(bool IsPlayer)
{
	if (weaponArray[0] == true)
	{
		if (IsPlayer)
		{
			return RandPlayerAttackDamageRifle;
		}
		return RandEnemyAttackDamageRifle;
	}
	if (weaponArray[1] == true)
	{
		if (IsPlayer)
		{
			return RandPlayerAttackDamageSniper;
		}
		return RandEnemyAttackDamageSniper;
	}
	if (weaponArray[2] == true)
	{
		if (IsPlayer)
		{
			return RandPlayerAttackDamagePistol;
		}
		return RandEnemyAttackDamagePistol;
	}
	if (weaponArray[3] == true)
	{
		if (IsPlayer)
		{
			return RandPlayerAttackDamageM249;
		}
		return RandEnemyAttackDamageM249;
	}
	return 0;
}


void APlayerCharacter::RemoveBossHPWidget()
{
	if (bossHPUI)
	{
		bossHPUI->RemoveFromParent();
	}
}

void APlayerCharacter::InfoWidgetUpdate()
{
}

float APlayerCharacter::DamageMultiplier()
{
	if (HeadsetEquipped)
	{
		return 1.16f;
	}
	return 1.f;
}

float APlayerCharacter::FireRateMultiplier()
{
	if (MaskEquipped)
	{
		return 1.23f;
	}
	return 1.f;
}

float APlayerCharacter::RecoilRateMultiplier()
{
	if (GoggleEquipped)
	{
		return 1.2f;
	}
	return 1.f;
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
	if (HasAuthority())
	{
		IsPlayerDeadImmediately = true;
	}
	if (IsLocallyControlled())
	{
		GetController()->SetIgnoreMoveInput(true);
		GetController()->SetIgnoreLookInput(true);
		if (AEclipsePlayerState* CachingPlayerState = Cast<AEclipsePlayerState>(GetPlayerState())) CachingPlayerState->InventoryCaching(this);
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
		APlayerCameraManager* playerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		// 카메라 페이드 연출
		playerCam->StartCameraFade(0, 1, 7.0, FLinearColor::Black, false, true);
		// 사망지점 전역변수에 캐싱
		DeathPosition = GetActorLocation();
	}
	FTimerHandle PlayerDeadHandle;
	GetWorld()->GetTimerManager().SetTimer(PlayerDeadHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		UGameplayStatics::PlaySound2D(GetWorld(), deathSound);
		IsPlayerDead = true;
	}), 3.f, false);

	// 몽타주 재생 중단
	StopAnimMontage();
	// 사망 몽타주 재생
	PlayAnimMontage(FullBodyMontage, 1, FName("Death"));

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

void APlayerCharacter::EquipHelmet(bool SoundBool)
{
	if (SoundBool)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), gearEquipSound, GetActorLocation());
	}
	HelmetSlot->SetVisibility(true);
	HelmetEquipped = true;
}

void APlayerCharacter::EquipHeadset(bool SoundBool)
{
	if (SoundBool)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), gearEquipSound, GetActorLocation());
	}
	HeadSetSlot->SetVisibility(true);
	HeadsetEquipped = true;
}

void APlayerCharacter::EquipMask(bool SoundBool)
{
	if (SoundBool)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), gearEquipSound, GetActorLocation());
	}
	MaskSlot->SetVisibility(true);
	MaskEquipped = true;
}

void APlayerCharacter::EquipGoggle(bool SoundBool)
{
	if (SoundBool)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), gearEquipSound, GetActorLocation());
	}
	GoggleSlot->SetVisibility(true);
	GoggleEquipped = true;
}

void APlayerCharacter::EquipArmor(bool SoundBool)
{
	if (SoundBool)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), gearEquipSound, GetActorLocation());
	}
	ArmorSlot->SetVisibility(true);
	ArmorEquipped = true;
	//curHP=FMath::Clamp(curHP+35, 0, 135);
	//maxHP=FMath::Clamp(maxHP+35, 0, 135);
}

void APlayerCharacter::UnEquipHelmet(bool SoundBool)
{
	if (SoundBool)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), gearUnequipSound, GetActorLocation());
	}
	HelmetSlot->SetVisibility(false);
	HelmetEquipped = false;
}

void APlayerCharacter::UnEquipHeadset(bool SoundBool)
{
	if (SoundBool)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), gearUnequipSound, GetActorLocation());
	}
	HeadSetSlot->SetVisibility(false);
	HeadsetEquipped = false;
}

void APlayerCharacter::UnEquipMask(bool SoundBool)
{
	if (SoundBool)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), gearUnequipSound, GetActorLocation());
	}
	MaskSlot->SetVisibility(false);
	MaskEquipped = false;
}

void APlayerCharacter::UnEquipGoggle(bool SoundBool)
{
	if (SoundBool)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), gearUnequipSound, GetActorLocation());
	}
	GoggleSlot->SetVisibility(false);
	GoggleEquipped = false;
}


void APlayerCharacter::UnEquipArmor(bool SoundBool)
{
	if (SoundBool)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), gearUnequipSound, GetActorLocation());
	}
	ArmorSlot->SetVisibility(false);
	ArmorEquipped = false;
}
