// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "ArmorActor.h"
#include "CrosshairWidget.h"
#include "Crunch.h"
#include "DamageWidget.h"
#include "DamageWidgetActor.h"
#include "EclipsePlayerController.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GoggleActor.h"
#include "Guardian.h"
#include "HackingConsole.h"
#include "HeadsetActor.h"
#include "HelmetActor.h"
#include "InformationWidget.h"
#include "M249Actor.h"
#include "M249MagActor.h"
#include "MaskActor.h"
#include "MedKitActor.h"
#include "MissionChecker.h"
#include "PlayerAnim.h"
#include "RifleActor.h"
#include "SniperActor.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PistolActor.h"
#include "PistolMagActor.h"
#include "RewardContainer.h"
#include "RifleMagActor.h"
#include "SniperMagActor.h"
#include "StageBoard.h"
#include "Stash.h"
#include "TabWidget.h"
#include "WeaponInfoWidget.h"
#include "XRLoadingScreenFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Eclipse/EclipseGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 360.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f; // The camera follows at this distance behind the character	
	//CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	sniperComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("sniperComp"));
	sniperComp->SetupAttachment(GetMesh(), FName("hand_r"));

	rifleComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("rifleComp"));
	rifleComp->SetupAttachment(GetMesh(), FName("hand_r"));

	pistolComp=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("pistolComp"));
	pistolComp->SetupAttachment(GetMesh(), FName("hand_l"));

	m249Comp= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m249Comp"));
	m249Comp->SetupAttachment(GetMesh(), FName("hand_r"));

	GoggleSlot= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GoggleSlot"));
	GoggleSlot->SetupAttachment(GetMesh(), FName("head"));

	HeadSetSlot= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadSetSlot"));
	HeadSetSlot->SetupAttachment(GetMesh(), FName("head"));

	MaskSlot= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MaskSlot"));
	MaskSlot->SetupAttachment(GetMesh(), FName("head"));

	HelmetSlot= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HelmetSlot"));
	HelmetSlot->SetupAttachment(GetMesh(), FName("head"));

	ArmorSlot= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmorSlot"));
	ArmorSlot->SetupAttachment(GetMesh(), FName("spine_03"));
	

	bUsingRifle=true;
	bUsingSniper=false;
	bUsingPistol=false;
	bUsingM249=false;
	
	weaponArray.Add(bUsingRifle); //0
	weaponArray.Add(bUsingSniper); //1
	weaponArray.Add(bUsingPistol); //2
	weaponArray.Add(bUsingM249); //3

	equippedWeaponStringArray.Add(FString("Rifle")); //0
	equippedWeaponStringArray.Add(FString("Pistol")); //1

	rifleComp->SetVisibility(true);
	sniperComp->SetVisibility(false);
	pistolComp->SetVisibility(false);
	m249Comp->SetVisibility(false);

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (AEclipsePlayerController* PlayerController = Cast<AEclipsePlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	pc=Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());
	gm=Cast<AEclipseGameMode>(GetWorld()->GetAuthGameMode());	

	bPlayerDeath=false;

	animInstance=Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());

	curHP=maxHP;

	// Timeline Binding
	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &APlayerCharacter::SetZoomValue);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}

	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	
	crosshairUI = CreateWidget<UCrosshairWidget>(GetWorld(), crosshairFactory);
	if(!crosshairUI->IsInViewport())
	{
		crosshairUI->AddToViewport();
	}

	infoWidgetUI = CreateWidget<UWeaponInfoWidget>(GetWorld(), infoWidgetFactory);

	sniperScopeUI=CreateWidget<UUserWidget>(GetWorld(), sniperScopeFactory);

	informationUI = CreateWidget<UInformationWidget>(GetWorld(), informationWidgetFactory);
	if(informationUI)
	{
		FTimerHandle respawnTimer;
		GetWorldTimerManager().SetTimer(respawnTimer, FTimerDelegate::CreateLambda([this]()->void
		{
			informationUI->owner=this;
			informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
			informationUI->BossCount->SetText(FText::AsNumber(BossCount));
			informationUI->ConsoleCount->SetText(FText::AsNumber(ConsoleCount));
			informationUI->UpdateAmmo();
			informationUI->UpdateAmmo_Secondary();
			if(!informationUI->IsInViewport())
			{
				informationUI->AddToViewport();
			}
		}), 0.5, false);
	}

	damageWidgetUI = CreateWidget<UDamageWidget>(GetWorld(), damageWidgetUIFactory);

	bossHPUI=CreateWidget<UBossHPWidget>(GetWorld(), bossHPWidgetFactory);

	StopAnimMontage();
	PlayAnimMontage(zoomingMontage, 1, FName("LevelStart"));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PlayerSpawnEmitter, GetActorLocation());
	AEclipsePlayerController* PlayerController = Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());
	if(PlayerController)
	{
		PlayerController->EnableInput(PlayerController);		
	}

	const auto cameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
	cameraManager->StopCameraFade();
	cameraManager->StartCameraFade(1.0, 0, 8.0, FColor::Black, false, true);

	GoggleSlot->SetVisibility(false);
	HelmetSlot->SetVisibility(false);
	HeadSetSlot->SetVisibility(false);
	MaskSlot->SetVisibility(false);
	ArmorSlot->SetVisibility(false);

	ApplyCachingValues();
	ApplyPouchCache();
	ApplyInventoryCache();
	ApplyStashCache();
	ApplyGearCache();
	ApplyMagCache();
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
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		//Zooming
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Started, this, &APlayerCharacter::Zoom);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Completed, this, &APlayerCharacter::ZoomRelease);

		//Running
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &APlayerCharacter::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &APlayerCharacter::RunRelease);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Crouching);

		//Change Weapon
		EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ChangeWeapon);

		//Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Fire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::FireRelease);

		//Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Reload);

		//Look Around
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnActionLookAroundPressed);
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Completed, this, &APlayerCharacter::OnActionLookAroundReleased);

		//Weapon Swap
		EnhancedInputComponent->BindAction(FirstWeaponSwapAction, ETriggerEvent::Started, this, &APlayerCharacter::SwapFirstWeapon);
		EnhancedInputComponent->BindAction(SecondWeaponSwapAction, ETriggerEvent::Started, this, &APlayerCharacter::SwapSecondWeapon);

		//Tab
		EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Started, this, &APlayerCharacter::Tab);

		//Q
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
	if(TabOn)
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
	if(TabOn)
	{
		return;
	}
	// Zooming Boolean
	isZooming=true;
	UGameplayStatics::PlaySound2D(GetWorld(), zoomSound);
	GetCharacterMovement()->MaxWalkSpeed=240.f;
	auto animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if(animInst)
	{
		animInst->bZooming=true;
	}
	// is using rifle
	if(weaponArray[0]==true)
	{
		if(animInst)
		{
			animInst->bRifleZooming=true;
		}
		Timeline.PlayFromStart();	
	}
	// is using sniper
	else if(weaponArray[1]==true)
	{
		isSniperZooming=true;
		crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
		const auto cameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
		cameraManager->StartCameraFade(1.0, 0.1, 3.0, FColor::Black, false, true);
		animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
		const auto controller = GetWorld()->GetFirstPlayerController();
		controller->PlayerCameraManager->StartCameraShake(sniperZoomingShake);
		// 카메라 줌 러프 타임라인 재생
		Timeline.PlayFromStart();
		sniperScopeUI->AddToViewport();
	}
	else if(weaponArray[3]==true)
	{
		if(animInst)
		{
			animInst->bM249Zooming=true;
		}
		Timeline.PlayFromStart();	
	}
	else
	{
		// 카메라 줌 러프 타임라인 재생
		Timeline.PlayFromStart();	
	}
}

void APlayerCharacter::ZoomRelease()
{
	if(TabOn)
	{
		return;
	}
	// Zooming Boolean
	isZooming = false;
	GetCharacterMovement()->MaxWalkSpeed=360.f;
	const auto animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if(animInst)
	{
		animInst->bZooming=false;
	}
	if(weaponArray[0]==true)
	{
		if(animInst)
		{
			animInst->bRifleZooming=false;
		}
		Timeline.ReverseFromEnd();
	}
	else if(weaponArray[1]==true)
	{
		isSniperZooming=false;
		const auto controller = GetWorld()->GetFirstPlayerController();
		controller->PlayerCameraManager->StopAllCameraShakes();
		if(GetMesh()->GetAnimInstance()->Montage_IsPlaying(zoomingMontage))
		{
			StopAnimMontage();
		}
		sniperScopeUI->RemoveFromParent();
		crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Visible);
		SniperZoomBool=false;
		SniperZoomOutBool=false;
		Timeline.ReverseFromEnd();
	}
	else if(weaponArray[3]==true)
	{
		if(animInst)
		{
			animInst->bM249Zooming=false;
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
	if (isSniperZooming)
	{
		if(!isSniperZoomed)
		{
			GetWorldTimerManager().ClearTimer(SniperZoomOutHandle);
			SniperZoomBool=true;
			SniperZoomOutBool=false;
			Timeline.Stop();
			FollowCamera->SetFieldOfView(40);
			Timeline.PlayFromStart();
			UGameplayStatics::PlaySound2D(GetWorld(), SniperZoomInSound);
			isSniperZoomed=true;
		}
		else if(isSniperZoomed)
		{
			GetWorldTimerManager().ClearTimer(SniperZoomHandle);
			SniperZoomOutBool=true;
			SniperZoomBool=false;
			Timeline.Stop();
			FollowCamera->SetFieldOfView(20);
			Timeline.PlayFromStart();
			UGameplayStatics::PlaySound2D(GetWorld(), SniperZoomOutSound);
			isSniperZoomed=false;
		}
		return;
	}
	if(isZooming)
	{
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed=520.f;
}

void APlayerCharacter::RunRelease()
{
	if(isZooming)
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
	if(curWeaponSlotNumber==1||isSniperZooming)
	{
		return;
	}
	auto animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if(animInst)
	{
		bool montagePlaying = animInst->IsAnyMontagePlaying();
		if(montagePlaying)
		{
			return;
		}
	}
	curWeaponSlotNumber=1;
	UGameplayStatics::PlaySound2D(GetWorld(), SwapSound);
	UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwapSound);
	if(equippedWeaponStringArray[0]==FString("Rifle"))
	{
		animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
		if(animInst)
		{
			// 애니메이션 블루프린트에 상태 전환 불리언 전달
			animInst->bPistol=false;
		}
		PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
		// Visibility 설정
		rifleComp->SetVisibility(true);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(false);
		// 무기 배열 설정
		weaponArray[0]=true;
		weaponArray[1]=false;
		weaponArray[2]=false;
		weaponArray[3]=false;

		informationUI->PlayAnimation(informationUI->WeaponSwap);
		informationUI->UpdateAmmo_Secondary();


	}
	else if(equippedWeaponStringArray[0]==FString("Sniper"))
	{
		animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
		if(animInst)
		{
			// 애니메이션 블루프린트에 상태 전환 불리언 전달
			animInst->bPistol=false;
		}
		PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
		// Visibility 설정
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(true);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(false);
		// 무기 배열 설정
		weaponArray[0]=false;
		weaponArray[1]=true;
		weaponArray[2]=false;
		weaponArray[3]=false;

		informationUI->PlayAnimation(informationUI->WeaponSwap);
		informationUI->UpdateAmmo_Secondary();

	}
	else if(equippedWeaponStringArray[0]==FString("Pistol"))
	{
		animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
		if(animInst)
		{
			// 애니메이션 블루프린트에 상태 전환 불리언 전달
			animInst->bPistol=true;
		}
		PlayAnimMontage(zoomingMontage, 1 , FName("PistolEquip"));
		// Visibility 설정
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(true);
		m249Comp->SetVisibility(false);
		// 무기 배열 설정
		weaponArray[0]=false;
		weaponArray[1]=false;
		weaponArray[2]=true;
		weaponArray[3]=false;

		informationUI->PlayAnimation(informationUI->WeaponSwap);
		informationUI->UpdateAmmo_Secondary();

	}
	else if(equippedWeaponStringArray[0]==FString("M249"))
	{
		animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
		if(animInst)
		{
			// 애니메이션 블루프린트에 상태 전환 불리언 전달
			animInst->bPistol=false;
		}
		PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
		// Visibility 설정
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(true);
		// 무기 배열 설정
		weaponArray[0]=false;
		weaponArray[1]=false;
		weaponArray[2]=false;
		weaponArray[3]=true;

		informationUI->PlayAnimation(informationUI->WeaponSwap);
		informationUI->UpdateAmmo_Secondary();

	}
}

void APlayerCharacter::SwapSecondWeapon()
{
	if(curWeaponSlotNumber==2||isSniperZooming)
	{
		return;
	}
	auto animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
	if(animInst)
	{
		bool montagePlaying = animInst->IsAnyMontagePlaying();
		if(montagePlaying)
		{
			return;
		}
	}
	curWeaponSlotNumber=2;
	UGameplayStatics::PlaySound2D(GetWorld(), SwapSound);
	UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwapSound);
	if(equippedWeaponStringArray[1]==FString("Rifle"))
	{
		animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
		if(animInst)
		{
			// 애니메이션 블루프린트에 상태 전환 불리언 전달
			animInst->bPistol=false;
		}
		PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
		// Visibility 설정
		rifleComp->SetVisibility(true);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(false);
		// 무기 배열 설정
		weaponArray[0]=true;
		weaponArray[1]=false;
		weaponArray[2]=false;
		weaponArray[3]=false;

		informationUI->PlayAnimation(informationUI->WeaponSwap);
		informationUI->UpdateAmmo_Secondary();


	}
	else if(equippedWeaponStringArray[1]==FString("Sniper"))
	{
		animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
		if(animInst)
		{
			// 애니메이션 블루프린트에 상태 전환 불리언 전달
			animInst->bPistol=false;
		}
		PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
		// Visibility 설정
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(true);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(false);
		// 무기 배열 설정
		weaponArray[0]=false;
		weaponArray[1]=true;
		weaponArray[2]=false;
		weaponArray[3]=false;

		informationUI->PlayAnimation(informationUI->WeaponSwap);
		informationUI->UpdateAmmo_Secondary();


	}
	else if(equippedWeaponStringArray[1]==FString("Pistol"))
	{
		animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
		if(animInst)
		{
			// 애니메이션 블루프린트에 상태 전환 불리언 전달
			animInst->bPistol=true;
		}
		PlayAnimMontage(zoomingMontage, 1 , FName("PistolEquip"));
		// Visibility 설정
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(true);
		m249Comp->SetVisibility(false);
		// 무기 배열 설정
		weaponArray[0]=false;
		weaponArray[1]=false;
		weaponArray[2]=true;
		weaponArray[3]=false;

		informationUI->PlayAnimation(informationUI->WeaponSwap);
		informationUI->UpdateAmmo_Secondary();

	}
	else if(equippedWeaponStringArray[1]==FString("M249"))
	{
		animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
		if(animInst)
		{
			// 애니메이션 블루프린트에 상태 전환 불리언 전달
			animInst->bPistol=false;
		}
		PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
		// Visibility 설정
		rifleComp->SetVisibility(false);
		sniperComp->SetVisibility(false);
		pistolComp->SetVisibility(false);
		m249Comp->SetVisibility(true);
		// 무기 배열 설정
		weaponArray[0]=false;
		weaponArray[1]=false;
		weaponArray[2]=false;
		weaponArray[3]=true;

		informationUI->PlayAnimation(informationUI->WeaponSwap);
		informationUI->UpdateAmmo_Secondary();

	}

}

void APlayerCharacter::Tab()
{

}

void APlayerCharacter::Q()
{
	PlayerDeath();
}


void APlayerCharacter::WeaponDetectionLineTrace()
{
	// 스나이퍼 줌 도중 교체 불가
	if(isSniperZooming||bEnding)
	{
		return;
	}
	FHitResult actorHitResult;
	FVector StartLoc = FollowCamera->GetComponentLocation();
	FVector EndLoc = StartLoc+FollowCamera->GetForwardVector()*500.0f;
	// 무기 액터 탐지 라인 트레이스
	bool bHit = GetWorld()->LineTraceSingleByChannel(actorHitResult, StartLoc, EndLoc, ECC_Visibility);
	if(bHit)
	{
		// 무기 액터 캐스팅
		rifleActor = Cast<ARifleActor>(actorHitResult.GetActor());
		sniperActor=Cast<ASniperActor>(actorHitResult.GetActor());
		pistolActor=Cast<APistolActor>(actorHitResult.GetActor());
		m249Actor=Cast<AM249Actor>(actorHitResult.GetActor());
		
		HackingConsole=Cast<AHackingConsole>(actorHitResult.GetActor());
		MissionChecker=Cast<AMissionChecker>(actorHitResult.GetActor());

		RifleMagActor = Cast<ARifleMagActor>(actorHitResult.GetActor());
		SniperMagActor = Cast<ASniperMagActor>(actorHitResult.GetActor());
		PistolMagActor = Cast<APistolMagActor>(actorHitResult.GetActor());
		M249MagActor = Cast<AM249MagActor>(actorHitResult.GetActor());

		GoggleActor= Cast<AGoggleActor>(actorHitResult.GetActor());
		MaskActor= Cast<AMaskActor>(actorHitResult.GetActor());
		HelmetActor= Cast<AHelmetActor>(actorHitResult.GetActor());
		HeadsetActor= Cast<AHeadsetActor>(actorHitResult.GetActor());
		ArmorActor= Cast<AArmorActor>(actorHitResult.GetActor());

		MedKitActor= Cast<AMedKitActor>(actorHitResult.GetActor());

		StageBoard= Cast<AStageBoard>(actorHitResult.GetActor());
		Stash=Cast<AStash>(actorHitResult.GetActor());
		
		// 라이플 탐지
		if(rifleActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				isCursorOnRifle=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				rifleActor->weaponMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(0);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(sniperActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				isCursorOnSniper=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				sniperActor->weaponMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(1);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(pistolActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				isCursorOnPistol=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				pistolActor->weaponMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(2);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(m249Actor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				isCursorOnM249=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				m249Actor->weaponMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(3);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(HackingConsole)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				HackingConsole->rewardMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(4);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(MissionChecker)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				MissionChecker->checkerMesh->SetRenderCustomDepth(true);
				if(BossCount>=1&&GuardianCount>=7)
				{
					infoWidgetUI->MissionCheck1->SetText(FText::FromString("Success"));
				}
				if(ConsoleCount>=5)
				{
					infoWidgetUI->MissionCheck2->SetText(FText::FromString("Success"));
				}
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(5);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(RifleMagActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				RifleMagActor->magMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(6);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(SniperMagActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				SniperMagActor->magMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(7);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(PistolMagActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				PistolMagActor->magMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(8);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(M249MagActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				M249MagActor->magMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(9);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(GoggleActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				GoggleActor->gearMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(10);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(HelmetActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				HelmetActor->gearMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(11);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(HeadsetActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				HeadsetActor->gearMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(12);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(MaskActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				MaskActor->gearMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(13);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(ArmorActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				ArmorActor->gearMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(14);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(MedKitActor)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				MedKitActor->consumeMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(15);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(StageBoard)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				StageBoard->boardMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(16);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else if(Stash)
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				// Render Custom Depth 활용한 무기 액터 외곽선 활성화
				Stash->stashMesh->SetRenderCustomDepth(true);
				// Widget Switcher 이용한 무기 정보 위젯 스위칭
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(17);
				// Radial Slider Value 초기화
				infoWidgetUI->weaponHoldPercent=0;
				// Weapon Info Widget 뷰포트에 배치
				infoWidgetUI->AddToViewport();
			}
		}
		else
		{
			// 1회 실행 불리언
			if(TickOverlapBoolean==true)
			{
				TickOverlapBoolean=false;
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
				if(bEndOverlapHit)
				{
					// 충돌 배열 순회
					for (int i = 0; i < HitObj.Num(); ++i)
					{
						// 무기 액터 캐스팅
						rifleActor = Cast<ARifleActor>(HitObj[i].GetActor());
						sniperActor=Cast<ASniperActor>(HitObj[i].GetActor());
						pistolActor=Cast<APistolActor>(HitObj[i].GetActor());
						m249Actor=Cast<AM249Actor>(HitObj[i].GetActor());
						HackingConsole=Cast<AHackingConsole>(HitObj[i].GetActor());
						MissionChecker=Cast<AMissionChecker>(HitObj[i].GetActor());
						RifleMagActor = Cast<ARifleMagActor>(HitObj[i].GetActor());
						SniperMagActor = Cast<ASniperMagActor>(HitObj[i].GetActor());
						PistolMagActor = Cast<APistolMagActor>(HitObj[i].GetActor());
						M249MagActor = Cast<AM249MagActor>(HitObj[i].GetActor());
						GoggleActor= Cast<AGoggleActor>(HitObj[i].GetActor());
						MaskActor= Cast<AMaskActor>(HitObj[i].GetActor());
						HelmetActor= Cast<AHelmetActor>(HitObj[i].GetActor());
						HeadsetActor= Cast<AHeadsetActor>(HitObj[i].GetActor());
						ArmorActor= Cast<AArmorActor>(HitObj[i].GetActor());
						MedKitActor= Cast<AMedKitActor>(HitObj[i].GetActor());
						StageBoard= Cast<AStageBoard>(HitObj[i].GetActor());
						Stash= Cast<AStash>(HitObj[i].GetActor());
						
						if(rifleActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							rifleActor->weaponMesh->SetRenderCustomDepth(false);
						}
						else if(sniperActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							sniperActor->weaponMesh->SetRenderCustomDepth(false);
						}
						else if(pistolActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							pistolActor->weaponMesh->SetRenderCustomDepth(false);
						}
						else if(m249Actor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							m249Actor->weaponMesh->SetRenderCustomDepth(false);
						}
						else if(HackingConsole)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							HackingConsole->rewardMesh->SetRenderCustomDepth(false);
						}
						else if(MissionChecker)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							MissionChecker->checkerMesh->SetRenderCustomDepth(false);
						}
						else if(RifleMagActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							RifleMagActor->magMesh->SetRenderCustomDepth(false);
						}
						else if(SniperMagActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							SniperMagActor->magMesh->SetRenderCustomDepth(false);
						}
						else if(PistolMagActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							PistolMagActor->magMesh->SetRenderCustomDepth(false);
						}
						else if(M249MagActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							M249MagActor->magMesh->SetRenderCustomDepth(false);
						}
						else if(GoggleActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							GoggleActor->gearMesh->SetRenderCustomDepth(false);
						}
						else if(HelmetActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							HelmetActor->gearMesh->SetRenderCustomDepth(false);
						}
						else if(HeadsetActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							HeadsetActor->gearMesh->SetRenderCustomDepth(false);
						}
						else if(MaskActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							MaskActor->gearMesh->SetRenderCustomDepth(false);
						}
						else if(ArmorActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							ArmorActor->gearMesh->SetRenderCustomDepth(false);
						}
						else if(MedKitActor)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							MedKitActor->consumeMesh->SetRenderCustomDepth(false);
						}
						else if(StageBoard)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							StageBoard->boardMesh->SetRenderCustomDepth(false);
						}
						else if(Stash)
						{
							// Render Custom Depth 활용한 무기 액터 외곽선 해제
							Stash->stashMesh->SetRenderCustomDepth(false);
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

void APlayerCharacter::SetBossHPWidget(AEnemy* enemy)
{
	if(enemy&&bossHPUI)
	{
		float bossHP = enemy->curHP*0.0001;
		bossHPUI->progressBar->SetPercent(bossHP);
		float bossShield = enemy->curShield*0.01;
		bossHPUI->shieldProgressBar->SetPercent(bossShield);
	}
}

void APlayerCharacter::SetDamageWidget(int damage, FVector spawnLoc, bool isShieldIconEnable, FLinearColor DamageTextColor)
{

	auto damWidget = GetWorld()->SpawnActor<ADamageWidgetActor>(damageWidgetFactory, spawnLoc+FVector(0, 0, 50), FRotator::ZeroRotator);
	if(damWidget)
	{
		auto widui = damWidget->DamageWidgetComponent->GetUserWidgetObject();
		if(widui)
		{
			damageWidgetUI=Cast<UDamageWidget>(widui);
			if(damageWidgetUI)
			{
				damageWidgetUI->damageText->SetColorAndOpacity(DamageTextColor);
				damageWidgetUI->damage=damage;
				if(isShieldIconEnable)
				{
					damageWidgetUI->ShieldImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}
				else
				{
					damageWidgetUI->ShieldImage->SetVisibility(ESlateVisibility::Hidden);
				}
				if(weaponArray[0]==true)
				{
					damageWidgetUI->rifleBulletImage->SetVisibility(ESlateVisibility::Visible);
					damageWidgetUI->sniperBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->pistolBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->M249BulletImage->SetVisibility(ESlateVisibility::Hidden);
				}
				else if(weaponArray[1]==true)
				{
					damageWidgetUI->rifleBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->sniperBulletImage->SetVisibility(ESlateVisibility::Visible);
					damageWidgetUI->pistolBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->M249BulletImage->SetVisibility(ESlateVisibility::Hidden);
				}
				else if(weaponArray[2]==true)
				{
					damageWidgetUI->rifleBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->sniperBulletImage->SetVisibility(ESlateVisibility::Hidden);
					damageWidgetUI->pistolBulletImage->SetVisibility(ESlateVisibility::Visible);
					damageWidgetUI->M249BulletImage->SetVisibility(ESlateVisibility::Hidden);
				}
				else if(weaponArray[3]==true)
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
	if(bEnding)
	{
		return;
	}		
	FHitResult actorHitResult;
	FVector StartLoc = FollowCamera->GetComponentLocation();
	FVector EndLoc = StartLoc+FollowCamera->GetForwardVector()*500.0f;
	// 무기 액터 탐지 라인 트레이스
	bool bHit = GetWorld()->LineTraceSingleByChannel(actorHitResult, StartLoc, EndLoc, ECC_Visibility);
	if(bHit)
	{
		// 무기 액터 캐스팅
		rifleActor = Cast<ARifleActor>(actorHitResult.GetActor());
		sniperActor=Cast<ASniperActor>(actorHitResult.GetActor());
		pistolActor=Cast<APistolActor>(actorHitResult.GetActor());
		m249Actor=Cast<AM249Actor>(actorHitResult.GetActor());
		
		HackingConsole=Cast<AHackingConsole>(actorHitResult.GetActor());
		MissionChecker=Cast<AMissionChecker>(actorHitResult.GetActor());

		RifleMagActor = Cast<ARifleMagActor>(actorHitResult.GetActor());
		SniperMagActor = Cast<ASniperMagActor>(actorHitResult.GetActor());
		PistolMagActor = Cast<APistolMagActor>(actorHitResult.GetActor());
		M249MagActor = Cast<AM249MagActor>(actorHitResult.GetActor());

		GoggleActor= Cast<AGoggleActor>(actorHitResult.GetActor());
		MaskActor= Cast<AMaskActor>(actorHitResult.GetActor());
		HelmetActor= Cast<AHelmetActor>(actorHitResult.GetActor());
		HeadsetActor= Cast<AHeadsetActor>(actorHitResult.GetActor());
		ArmorActor= Cast<AArmorActor>(actorHitResult.GetActor());

		MedKitActor= Cast<AMedKitActor>(actorHitResult.GetActor());
		StageBoard= Cast<AStageBoard>(actorHitResult.GetActor());
		Stash= Cast<AStash>(actorHitResult.GetActor());
		
		// 라이플로 교체
		if(rifleActor)
		{
			// 라이플을 사용하지 않을 때만 교체
			if(weaponArray[0]==false)
			{
				// 키다운 시간 동안 Radial Slider 게이지 상승
				infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
				// 게이지가 모두 채워졌을 때
				if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
				{
					infoWidgetUI->weaponHoldPercent=0;
					UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwapSound);
					// 무기 정보 위젯 제거
					infoWidgetUI->RemoveFromParent();
					// 무기 교체 Montage 재생
					PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
					// 교체 대상 무기 액터 파괴
					rifleActor->Destroy();
					// 액터 스폰 지점 할당
					FVector spawnPosition = GetMesh()->GetSocketLocation(FName("hand_r"));
					FRotator spawnRotation = FRotator::ZeroRotator;
					FActorSpawnParameters param;
					param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					// 스나이퍼를 사용중일 때
					if(weaponArray[1]==true)
					{
						// 사용중인 무기 액터 스폰
						sniperActor = GetWorld()->SpawnActor<ASniperActor>(sniperFactory, spawnPosition, spawnRotation);
					}
					// 권총을 사용중일 때
					else if(weaponArray[2]==true)
					{
						// 애니메이션 인스턴스 캐스팅
						auto animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
						if(animInst)
						{
							// 애니메이션 블루프린트에 상태 전환 불리언 전달
							animInst->bPistol=false;
						}
						// 사용중인 무기 액터 스폰
						pistolActor = GetWorld()->SpawnActor<APistolActor>(pistolFactory, spawnPosition, spawnRotation);
					}
					else if(weaponArray[3]==true)
					{
						// 사용중인 무기 액터 스폰
						m249Actor = GetWorld()->SpawnActor<AM249Actor>(M249Factory, spawnPosition, spawnRotation);
					}
					// 현재 활성화된 슬롯이 1번이라면
					if(curWeaponSlotNumber==1)
					{
						// 장착 무기 이름 배열에 할당
						equippedWeaponStringArray[0]=FString("Rifle");
					}
					// 현재 활성화된 슬롯이 2번이라면
					else if(curWeaponSlotNumber==2)
					{
						// 장착 무기 이름 배열에 할당
						equippedWeaponStringArray[1]=FString("Rifle");
					}
					// Visibility 설정
					rifleComp->SetVisibility(true);
					sniperComp->SetVisibility(false);
					pistolComp->SetVisibility(false);
					m249Comp->SetVisibility(false);
					// 무기 배열 설정
					weaponArray[0]=true;
					weaponArray[1]=false;
					weaponArray[2]=false;
					weaponArray[3]=false;
				}
			}
		}
		// 스나이퍼로 교체
		else if(sniperActor)
		{
			// 스나이퍼를 사용하지 않을 때만 교체
			if(weaponArray[1]==false)
			{
				infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
				if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
				{
					infoWidgetUI->weaponHoldPercent=0;
					UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwapSound);
					infoWidgetUI->RemoveFromParent();
					PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
					sniperActor->Destroy();
					FVector spawnPosition = GetMesh()->GetSocketLocation(FName("hand_r"));
					FRotator spawnRotation = FRotator::ZeroRotator;
					FActorSpawnParameters param;
					param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
					if(weaponArray[0]==true)
					{
						rifleActor = GetWorld()->SpawnActor<ARifleActor>(rifleFactory, spawnPosition, spawnRotation);
					}
					else if(weaponArray[2]==true)
					{
						auto animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
						if(animInst)
						{
							animInst->bPistol=false;
						}
						pistolActor = GetWorld()->SpawnActor<APistolActor>(pistolFactory, spawnPosition, spawnRotation);
					}
					else if(weaponArray[3]==true)
					{
						m249Actor = GetWorld()->SpawnActor<AM249Actor>(M249Factory, spawnPosition, spawnRotation);
					}
					if(curWeaponSlotNumber==1)
					{
						equippedWeaponStringArray[0]=FString("Sniper");
					}
					else if(curWeaponSlotNumber==2)
					{
						equippedWeaponStringArray[1]=FString("Sniper");
					}			
					rifleComp->SetVisibility(false);
					sniperComp->SetVisibility(true);
					pistolComp->SetVisibility(false);
					m249Comp->SetVisibility(false);

					weaponArray[0]=false;
					weaponArray[1]=true;
					weaponArray[2]=false;
					weaponArray[3]=false;

				}
			}
		}
		// 권총으로 교체
		else if(pistolActor)
		{
			// 권총을 사용하지 않을 때만 교체
			if(weaponArray[2]==false)
			{
				infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
				if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
				{
					infoWidgetUI->weaponHoldPercent=0;
					UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwapSound);
					infoWidgetUI->RemoveFromParent();
					PlayAnimMontage(zoomingMontage, 1 , FName("PistolEquip"));
					auto animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
					if(animInst)
					{
						animInst->bPistol=true;
					}
					pistolActor->Destroy();
					FVector spawnPosition = GetMesh()->GetSocketLocation(FName("hand_r"));
					FRotator spawnRotation = FRotator::ZeroRotator;
					FActorSpawnParameters param;
					param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;		
					if(weaponArray[0]==true)
					{
						rifleActor = GetWorld()->SpawnActor<ARifleActor>(rifleFactory, spawnPosition, spawnRotation);
					}
					else if(weaponArray[1]==true)
					{
						sniperActor = GetWorld()->SpawnActor<ASniperActor>(sniperFactory, spawnPosition, spawnRotation);
					}
					else if(weaponArray[3]==true)
					{
						m249Actor = GetWorld()->SpawnActor<AM249Actor>(M249Factory, spawnPosition, spawnRotation);
					}
					if(curWeaponSlotNumber==1)
					{
						equippedWeaponStringArray[0]=FString("Pistol");
					}
					else if(curWeaponSlotNumber==2)
					{
						equippedWeaponStringArray[1]=FString("Pistol");
					}				
					rifleComp->SetVisibility(false);
					sniperComp->SetVisibility(false);
					pistolComp->SetVisibility(true);
					m249Comp->SetVisibility(false);
				
					weaponArray[0]=false;
					weaponArray[1]=false;
					weaponArray[2]=true;
					weaponArray[3]=false;
				}
			}
		}
		// M249로 교체
		else if(m249Actor)
		{
			// M249을 사용하지 않을 때만 교체
			if(weaponArray[3]==false)
			{
				infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
				if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
				{
					infoWidgetUI->weaponHoldPercent=0;
					UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwapSound);
					infoWidgetUI->RemoveFromParent();
					PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
					m249Actor->Destroy();
					FVector spawnPosition = GetMesh()->GetSocketLocation(FName("hand_r"));
					FRotator spawnRotation = FRotator::ZeroRotator;
					FActorSpawnParameters param;
					param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;		
					if(weaponArray[0]==true)
					{
						rifleActor = GetWorld()->SpawnActor<ARifleActor>(rifleFactory, spawnPosition, spawnRotation);
					}
					else if(weaponArray[1]==true)
					{
						sniperActor = GetWorld()->SpawnActor<ASniperActor>(sniperFactory, spawnPosition, spawnRotation);
					}
					else if(weaponArray[2]==true)
					{
						auto animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
						if(animInst)
						{
							animInst->bPistol=false;
						}
						pistolActor = GetWorld()->SpawnActor<APistolActor>(pistolFactory, spawnPosition, spawnRotation);
					}
					if(curWeaponSlotNumber==1)
					{
						equippedWeaponStringArray[0]=FString("M249");
					}
					else if(curWeaponSlotNumber==2)
					{
						equippedWeaponStringArray[1]=FString("M249");
					}				
					rifleComp->SetVisibility(false);
					sniperComp->SetVisibility(false);
					pistolComp->SetVisibility(false);
					m249Comp->SetVisibility(true);
				
					weaponArray[0]=false;
					weaponArray[1]=false;
					weaponArray[2]=false;
					weaponArray[3]=true;
				}
			}
		}
		else if(HackingConsole)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
				infoWidgetUI->RemoveFromParent();
				PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
				HackingConsole->AddInventory();
				HackingConsole->Destroy();
				ConsoleCount++;
				informationUI->ConsoleCount->SetText(FText::AsNumber(ConsoleCount));
			}
		}
		else if(MissionChecker)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				if(GuardianCount>=7&&ConsoleCount>=5&&BossCount>=1)
				{
					infoWidgetUI->weaponHoldPercent=0;
					bEnding=true;
					auto playerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
					playerCam->StartCameraFade(0, 1, 7.0, FLinearColor::Black, false, true);
					StopAnimMontage();
					GetCharacterMovement()->StopActiveMovement();
					GetCharacterMovement()->DisableMovement();
					auto spawnTrans = this->GetTransform();
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), recallParticle, spawnTrans);
					PlayAnimMontage(zoomingMontage, 1, FName("LevelEnd"));
					UGameplayStatics::PlaySound2D(GetWorld(), PortalSound);
					bUseControllerRotationYaw=false;
					infoWidgetUI->RemoveFromParent();
					informationUI->RemoveFromParent();
					crosshairUI->RemoveFromParent();
					FTimerHandle endHandle;
					GetWorldTimerManager().SetTimer(endHandle, FTimerDelegate::CreateLambda([this]()->void
					{						
						PouchCaching();
						InventoryCaching();
						GearCaching();
						MagCaching();
						UGameplayStatics::OpenLevel(GetWorld(), FName("Safe_House"));

						//auto pc = GetWorld()->GetFirstPlayerController();
						//TEnumAsByte<EQuitPreference::Type> types = EQuitPreference::Quit;
						//UKismetSystemLibrary::QuitGame(GetWorld(),pc, types, false);
					}), 9.f, false);
				}
				else
				{
					infoWidgetUI->PlayAnimation(infoWidgetUI->LackMission);
					infoWidgetUI->weaponHoldPercent=0;
				}
			}
		}
		else if(RifleMagActor)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
				infoWidgetUI->RemoveFromParent();
				PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
				RifleMagActor->AddInventory();
				RifleMagActor->Destroy();
			}
		}
		else if(SniperMagActor)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
				infoWidgetUI->RemoveFromParent();
				PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
				SniperMagActor->AddInventory();
				SniperMagActor->Destroy();
			}
		}
		else if(PistolMagActor)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
				infoWidgetUI->RemoveFromParent();
				PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
				PistolMagActor->AddInventory();
				PistolMagActor->Destroy();
			}
		}
		else if(M249MagActor)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
				infoWidgetUI->RemoveFromParent();
				PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
				M249MagActor->AddInventory();
				M249MagActor->Destroy();
			}
		}
		else if(GoggleActor)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
				infoWidgetUI->RemoveFromParent();
				PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
				GoggleActor->AddInventory();
				GoggleActor->Destroy();
			}
		}
		else if(HelmetActor)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
				infoWidgetUI->RemoveFromParent();
				PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
				HelmetActor->AddInventory();
				HelmetActor->Destroy();
			}
		}
		else if(HeadsetActor)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
				infoWidgetUI->RemoveFromParent();
				PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
				HeadsetActor->AddInventory();
				HeadsetActor->Destroy();
			}
		}
		else if(MaskActor)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
				infoWidgetUI->RemoveFromParent();
				PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
				MaskActor->AddInventory();
				MaskActor->Destroy();
			}
		}
		else if(ArmorActor)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
				infoWidgetUI->RemoveFromParent();
				PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
				ArmorActor->AddInventory();
				ArmorActor->Destroy();
			}
		}
		else if(MedKitActor)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				UGameplayStatics::PlaySound2D(GetWorld(), PickUpSound);
				infoWidgetUI->RemoveFromParent();
				PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
				MedKitActor->AddInventory();
				MedKitActor->Destroy();
			}
		}
		else if(StageBoard)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				bEnding=true;
				auto playerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
				playerCam->StartCameraFade(0, 1, 7.0, FLinearColor::Black, false, true);
				StopAnimMontage();
				GetCharacterMovement()->StopActiveMovement();
				GetCharacterMovement()->DisableMovement();
				auto spawnTrans = this->GetTransform();
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), recallParticle, spawnTrans);
				PlayAnimMontage(zoomingMontage, 1, FName("LevelEnd"));
				UGameplayStatics::PlaySound2D(GetWorld(), PortalSound);
				bUseControllerRotationYaw=false;
				infoWidgetUI->RemoveFromParent();
				informationUI->RemoveFromParent();
				crosshairUI->RemoveFromParent();
				FTimerHandle endHandle;
				GetWorldTimerManager().SetTimer(endHandle, FTimerDelegate::CreateLambda([this]()->void
				{					
					PouchCaching();
					InventoryCaching();
					StashCaching();
					GearCaching();
					MagCaching();
					UGameplayStatics::OpenLevel(GetWorld(), FName("Map_BigStarStation"));
				}), 9.f, false);				
			}
		}
		else if(Stash)
		{
			infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.015, 0, 1);
			if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
			{
				infoWidgetUI->weaponHoldPercent=0;
				if(bStashWidgetOn==false&&pc)
				{
					bStashWidgetOn=true;
					UGameplayStatics::PlaySound2D(GetWorld(), tabSound);
					infoWidgetUI->RemoveFromParent();
					pc->SetShowMouseCursor(true);
					StashWidgetOnViewport();
				}					
			}
		}
	}	
}

void APlayerCharacter::Reload()
{
	bool animPlay = animInstance->IsAnyMontagePlaying();
	if(animPlay==false)
	{
		if(weaponArray[0]==true&&curRifleAmmo<40+SetRifleAdditionalMagazine()&&maxRifleAmmo>0)
		{
			crosshairUI->PlayAnimation(crosshairUI->ReloadAnimation);
			UGameplayStatics::PlaySound2D(GetWorld(), RifleReloadSound);
			PlayAnimMontage(zoomingMontage, 1, FName("Reload"));
		}
		else if(weaponArray[1]==true&&curSniperAmmo<5+SetSniperAdditionalMagazine()&&maxSniperAmmo>0)
		{
			crosshairUI->PlayAnimation(crosshairUI->ReloadAnimation);
			PlayAnimMontage(zoomingMontage, 1, FName("Reload"));
		}
		else if(weaponArray[2]==true&&curPistolAmmo<8+SetPistolAdditionalMagazine()&&maxPistolAmmo>0)
		{
			crosshairUI->PlayAnimation(crosshairUI->ReloadAnimation);
			PlayAnimMontage(zoomingMontage, 1, FName("Reload"));
		}
		else if(weaponArray[3]==true&&curM249Ammo<100+SetM249AdditionalMagazine()&&maxM249Ammo>0)
		{
			crosshairUI->PlayAnimation(crosshairUI->ReloadAnimation);
			PlayAnimMontage(zoomingMontage, 1, FName("Reload"));
		}
	}
}

void APlayerCharacter::SetZoomValue(float Value)
{

	if(weaponArray[1]==true&&!SniperZoomBool&&!SniperZoomOutBool)
	{
		// 타임라인 Float Curve 에 따른 Lerp
		auto lerp=UKismetMathLibrary::Lerp(90,40,Value);
		// 해당 Lerp값 Arm Length에 적용
		FollowCamera->SetFieldOfView(lerp);
	}
	else if(weaponArray[1]==false)
	{
		// 타임라인 Float Curve 에 따른 Lerp
		auto lerp=UKismetMathLibrary::Lerp(200,100,Value);
		// 해당 Lerp값 Arm Length에 적용
		CameraBoom->TargetArmLength=lerp;
	}
	else if(SniperZoomBool)
	{
		auto lerp = UKismetMathLibrary::Lerp(40,20,Value);
		FollowCamera->SetFieldOfView(lerp);
		GetWorldTimerManager().SetTimer(SniperZoomHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			SniperZoomBool=false;
		}), 1.0f, false);
	}
	else if(SniperZoomOutBool)
	{
		auto lerp = FollowCamera->FieldOfView = UKismetMathLibrary::Lerp(20,40,Value);
		FollowCamera->SetFieldOfView(lerp);
		GetWorldTimerManager().SetTimer(SniperZoomOutHandle, FTimerDelegate::CreateLambda([this]()->void
	{
		SniperZoomOutBool=false;
	}), 1.0f, false);
	}
}

void APlayerCharacter::CachingValues()
{
	if(gm)
	{
		 gm->ConsoleCount = ConsoleCount;

		 gm->GuardianCount = GuardianCount;

		 gm->BossCount = BossCount;

		 gm->curRifleAmmo = curRifleAmmo;

		 gm->curSniperAmmo = curSniperAmmo;

		 gm->curPistolAmmo = curPistolAmmo;

		 gm->curM249Ammo = curM249Ammo;

		 gm->maxRifleAmmo = maxRifleAmmo;

		 gm->maxSniperAmmo = maxSniperAmmo;

		 gm->maxPistolAmmo = maxPistolAmmo;

		 gm->maxM249Ammo = maxM249Ammo;
	}
}

void APlayerCharacter::ApplyCachingValues()
{
	if(gm)
	{
		ConsoleCount = gm->ConsoleCount;

		GuardianCount = gm->GuardianCount;

		BossCount = gm->BossCount;

		curRifleAmmo = gm->curRifleAmmo;

		curSniperAmmo = gm->curSniperAmmo;

		curPistolAmmo = gm->curPistolAmmo;

		curM249Ammo = gm->curM249Ammo;

		maxRifleAmmo = gm->maxRifleAmmo;

		maxSniperAmmo = gm->maxSniperAmmo;

		maxPistolAmmo = gm->maxPistolAmmo;

		maxM249Ammo = gm->maxM249Ammo;
	}
}

void APlayerCharacter::Damaged(int damage)
{
	if(curHP<=damage)
	{
		curHP=0;
		PlayerDeath();
	}
	else
	{
		curHP = FMath::Clamp(curHP-damage, 0, maxHP);
		StopAnimMontage();
		PlayAnimMontage(zoomingMontage, 1, FName("Damaged"));
		const auto controller = GetWorld()->GetFirstPlayerController();
		controller->PlayerCameraManager->StartCameraShake(PlayerDamagedShake);
		UpdateTabWidget();
	}
}



int32 APlayerCharacter::SetRifleAdditionalMagazine()
{
	if(bRifleAdditionalMag)
	{
		return 15;
	}
	return 0;
}

int32 APlayerCharacter::SetSniperAdditionalMagazine()
{
	if(bSniperAdditionalMag)
	{
		return 2;
	}
	return 0;
}

int32 APlayerCharacter::SetPistolAdditionalMagazine()
{
	if(bPistolAdditionalMag)
	{
		return 4;
	}
	return 0;
}

int32 APlayerCharacter::SetM249AdditionalMagazine()
{
	if(bM249AdditionalMag)
	{
		return 30;
	}
	return 0;
}

void APlayerCharacter::SetRifleAdditionalMagazineSlot()
{
	bRifleAdditionalMag=true;
}

void APlayerCharacter::SetSniperAdditionalMagazineSlot()
{
	bSniperAdditionalMag=true;
}

void APlayerCharacter::SetPistolAdditionalMagazineSlot()
{
	bPistolAdditionalMag=true;
}

void APlayerCharacter::SetM249AdditionalMagazineSlot()
{
	bM249AdditionalMag=true;
}



void APlayerCharacter::UnSetRifleAdditionalMagazineSlot()
{
	bRifleAdditionalMag=false;
	// if(curRifleAmmo>=15)
	// {
	// 	curRifleAmmo-=15;
	// 	maxRifleAmmo+=15;
	// }
	// else
	// {
	// 	curRifleAmmo=0;
	// 	maxRifleAmmo+=curRifleAmmo;
	// }
}

void APlayerCharacter::UnSetSniperAdditionalMagazineSlot()
{
	bSniperAdditionalMag=false;
}

void APlayerCharacter::UnSetPistolAdditionalMagazineSlot()
{
	bPistolAdditionalMag=false;
}

void APlayerCharacter::UnSetM249AdditionalMagazineSlot()
{
	bM249AdditionalMag=false;
}



void APlayerCharacter::Fire()
{	
	if(!CanShoot||isRunning||TabOn||bEnding)
	{
		return;
	}
	// 라이플을 들고 있으면서 사격 가능한 상태라면
	if(weaponArray[0]==true)
	{
		if(curRifleAmmo>0)
		{
			// Clamp를 통한 탄약 수 차감
			curRifleAmmo = FMath::Clamp(curRifleAmmo-1, 0, 40+SetRifleAdditionalMagazine());
			UE_LOG(LogTemp, Warning, TEXT("Cur Rifle Bullet : %d"), curRifleAmmo)
			FVector startLoc = FollowCamera->GetComponentLocation();
			FVector EndLoc = startLoc + FollowCamera->GetForwardVector()*10000.0f;
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // LineTrace로 히트 가능한 오브젝트 유형들.
			TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
			TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);
			TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
			TEnumAsByte<EObjectTypeQuery> PhysicsBody = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody);
			TEnumAsByte<EObjectTypeQuery> Destructible = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Destructible);
			ObjectTypes.Add(WorldStatic);
			ObjectTypes.Add(WorldDynamic);
			ObjectTypes.Add(Pawn);
			ObjectTypes.Add(PhysicsBody);
			ObjectTypes.Add(Destructible);
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this); // LineTrace에서 제외할 대상
			FHitResult rifleHitResult;
			auto particleTrans = rifleComp->GetSocketTransform(FName("RifleFirePosition"));
			particleTrans.SetScale3D(FVector(0.7));
			auto particleLoc2 = rifleComp->GetSocketLocation(FName("RifleFirePosition"));
			auto particleRot2 = rifleComp->GetSocketRotation(FName("RifleFirePosition"))+FRotator(0, 0, 90);
			auto particleTrans2=UKismetMathLibrary::MakeTransform(particleLoc2, particleRot2, FVector(0.4));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RifleFireParticle, particleTrans);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RifleFireParticle2, particleTrans2);
			FActorSpawnParameters param;
			param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			auto spawnTrans = rifleComp->GetSocketTransform(FName("BulletShell"));
			auto bulletShell = GetWorld()->SpawnActor<AActor>(BulletShellFactory, spawnTrans);
			bulletShell->SetLifeSpan(5.0f);
			auto bulSoundLoc = GetActorLocation()*FVector(0, 0, -80);
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), RifleBulletShellDropSound, bulSoundLoc, FRotator::ZeroRotator, 0.4, 1, 0);
			UGameplayStatics::PlaySound2D(GetWorld(), RifleFireSound);
			auto controller = GetWorld()->GetFirstPlayerController();
			controller->PlayerCameraManager->StartCameraShake(rifleFireShake);
			bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),startLoc, EndLoc, ObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::None, rifleHitResult, true);
			if(bHit)
			{
				// Enemy Casting
				AEnemy* enemy=Cast<AEnemy>(rifleHitResult.GetActor());
				// Enemy FSM Casting
				UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
				// Reward Container Casting
				ARewardContainer* rewardContainer=Cast<ARewardContainer>(rifleHitResult.GetActor());
				if(fsm&&enemy)
				{
					auto guardian=Cast<AGuardian>(enemy);
					auto crunch = Cast<ACrunch>(enemy);
					if(guardian)
					{
						bGuardian=true;
					}
					else if(crunch)
					{
						bCrunch=true;
					}
					// 이미 죽지 않은 적에게만 실행
					if(enemy->bDeath==false)
					{
						hitActors = rifleHitResult.GetActor();
						auto hitBone = rifleHitResult.BoneName;
						auto hitLoc = rifleHitResult.Location;
						auto hitRot = UKismetMathLibrary::Conv_VectorToRotator(rifleHitResult.ImpactNormal);
						// 헤드샷 적중
						if(hitBone==FName("head"))
						{
							// 반환값 float의 데미지 증가 처리 함수와 곱연산
							auto min = FMath::RoundFromZero(120*DamageMultiplier());
							auto max = FMath::RoundFromZero(180*DamageMultiplier());
							// 헤드 데미지 랜덤 산출
							randRifleHeadDamage = FMath::RandRange(min, max);
							// 이번 공격에 적이 죽는다면
							if(enemy->curHP<=randRifleHeadDamage)
							{
								// Enemy Kill 위젯 애니메이션 재생
								crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
								// 킬 사운드 재생
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
								// 킬 파티클 스폰
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.0f));
								// FSM에 있는 Damage Process 호출		
								fsm->OnDamageProcess(randRifleHeadDamage);								
								SetDamageWidget(randRifleHeadDamage, hitLoc, false, FLinearColor::Yellow);								
								// 헤드 적중 데미지 프로세스 호출
								enemy->OnHeadDamaged();								
								// 사망 불리언 활성화
								enemy->bDeath=true;
								if(bGuardian)
								{
									GuardianCount++;
									informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
									// 전리품 드롭
									enemy->DropReward();
								}
								else if(bCrunch)
								{									
									BossCount++;
									informationUI->BossCount->SetText(FText::AsNumber(BossCount));
									SetBossHPWidget(enemy);
									enemy->DropReward();
									FTimerHandle removeHandle;
									GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
								}
							}
							// 이번 공격에 적이 죽지 않는다면
							else
							{
								// 헤드 적중 위젯 애니메이션 재생
								crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
								// 헤드 적중 사운드 재생
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHeadHitSound, hitLoc);
								// 헤드 적중 파티클 스폰
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.0f));
								// 적중 대상이 보스라면
								if(bCrunch)
								{
									// 실드가 파괴된 상태라면
									if(enemy->isShieldBroken)
									{
										// FSM에 있는 Damage Process 호출		
										fsm->OnDamageProcess(randRifleHeadDamage);
										if(enemy->isStunned)
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randRifleHeadDamage*2, hitLoc, false, FLinearColor::Red);
										}
										else
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randRifleHeadDamage, hitLoc, false, FLinearColor::Yellow);
										}										
										// 헤드 적중 데미지 프로세스 호출
										enemy->OnHeadDamaged();
									}
									// 실드가 파괴되지 않은 상태라면
									else
									{
										auto EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
										EmitterTrans.SetScale3D(FVector(1.3));
										UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
										// FSM에 있는 Damage Process 호출		
										fsm->OnShieldDamageProcess(randRifleHeadDamage);
										// 데미지 위젯에 피해 값과 적 위치벡터 할당
										SetDamageWidget(randRifleHeadDamage/20, hitLoc, true, FLinearColor::Yellow);
										// 헤드 적중 데미지 프로세스 호출
										enemy->OnHeadDamaged();
									}
									SetBossHPWidget(enemy);
								}
								// 보스가 아니라면
								else
								{
									// FSM에 있는 Damage Process 호출		
									fsm->OnDamageProcess(randRifleHeadDamage);
									// 데미지 위젯에 피해 값과 적 위치벡터 할당
									SetDamageWidget(randRifleHeadDamage, hitLoc, false, FLinearColor::Yellow);
									// 헤드 적중 데미지 프로세스 호출
									enemy->OnHeadDamaged();
								}
							}
						}
						// 일반 적중
						else
						{
							// 반환값 float의 데미지 증가 처리 함수와 곱연산
							auto min = FMath::RoundFromZero(60*DamageMultiplier());
							auto max = FMath::RoundFromZero(90*DamageMultiplier());
							// 일반 데미지 랜덤 산출
							randRifleDamage = FMath::RandRange(min, max);
							// 이번 공격에 적이 죽는다면
							if(enemy->curHP<=randRifleDamage)
							{
								// Enemy Kill 위젯 애니메이션 재생
								crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
								// 킬 사운드 재생
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
								// 킬 파티클 스폰
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.0f));
								// FSM에 있는 Damage Process 호출		
								fsm->OnDamageProcess(randRifleDamage);
								SetDamageWidget(randRifleDamage, hitLoc, false, FLinearColor::White);

								// 일반 적중 데미지 프로세스 호출
								enemy->OnDamaged();
								
								// 사망 불리언 활성화
								enemy->bDeath=true;
								if(bGuardian)
								{
									GuardianCount++;
									informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
									// 전리품 드롭
									enemy->DropReward();

								}
								else if(bCrunch)
								{
									BossCount++;
									informationUI->BossCount->SetText(FText::AsNumber(BossCount));
									SetBossHPWidget(enemy);
									// 전리품 드롭
									enemy->DropReward();
									FTimerHandle removeHandle;
									GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
								}
							}
							else
							{
								// 적중 위젯 애니메이션 재생
								crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
								// 적중 사운드 재생
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, hitLoc);
								// 적중 파티클 스폰
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(0.5f));

								// 적중 대상이 보스라면
								if(bCrunch)
								{
									// 실드가 파괴된 상태라면
									if(enemy->isShieldBroken)
									{
										// FSM에 있는 Damage Process 호출		
										fsm->OnDamageProcess(randRifleDamage);
										if(enemy->isStunned)
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randRifleDamage*2, hitLoc, false, FLinearColor::Red);
										}
										else
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randRifleDamage, hitLoc, false, FLinearColor::White);
										}
										// 일반 적중 데미지 프로세스 호출
										enemy->OnDamaged();
									}
									// 실드가 있는 상태라면
									else
									{
										auto EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
										EmitterTrans.SetScale3D(FVector(1.3));
										UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
										// FSM에 있는 Damage Process 호출		
										fsm->OnShieldDamageProcess(randRifleDamage);
										// 데미지 위젯에 피해 값과 적 위치벡터 할당
										SetDamageWidget(randRifleDamage/20, hitLoc, true, FLinearColor::White);
										// 일반 적중 데미지 프로세스 호출
										enemy->OnDamaged();
									}
									SetBossHPWidget(enemy);
								}
								// 보스가 아니라면
								else
								{
									// FSM에 있는 Damage Process 호출		
									fsm->OnDamageProcess(randRifleDamage);
									// 데미지 위젯에 피해 값과 적 위치벡터 할당
									SetDamageWidget(randRifleDamage, hitLoc, false, FLinearColor::White);
									// 일반 적중 데미지 프로세스 호출
									enemy->OnDamaged();
								}
							}
						}
					}
					bGuardian=false;
					bCrunch=false;
				}
				else if(rewardContainer)
				{
					if(!rewardContainer->bDestroyed)
					{
						auto hitLoc = rifleHitResult.Location;
						crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, hitLoc);
						if(rewardContainer->curBoxHP<=1)
						{
							rewardContainer->BoxDestroyed();
							rewardContainer->containerMesh->SetSimulatePhysics(true);
							ContainerLoc = rewardContainer->GetActorLocation();
							containerDele.ExecuteIfBound();
						}
						else
						{
							rewardContainer->curBoxHP=FMath::Clamp(rewardContainer->curBoxHP-1, 0, 10);
						}
					}
				}
				else
				{
					auto decalRot = UKismetMathLibrary::Conv_VectorToRotator(rifleHitResult.ImpactNormal);
					auto decalLoc = rifleHitResult.Location;
					auto decalTrans = UKismetMathLibrary::MakeTransform(decalLoc, decalRot);
					GetWorld()->SpawnActor<AActor>(ShotDecalFactory, decalTrans);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, decalLoc, decalRot+FRotator(-90, 0, 0), FVector(0.5f));
				}
				auto randF = UKismetMathLibrary::RandomFloatInRange(-0.3*RecoilRateMultiplier(), -0.5*RecoilRateMultiplier());
				auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.3*RecoilRateMultiplier(), 0.3*RecoilRateMultiplier());
				AddControllerPitchInput(randF);
				AddControllerYawInput(randF2);						
				CanShoot=false;
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), 1/(BulletsPerSecRifle*FireRateMultiplier()), false);
			}
			else
			{
				auto randF = UKismetMathLibrary::RandomFloatInRange(-0.3*RecoilRateMultiplier(), -0.5*RecoilRateMultiplier());
				auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.3*RecoilRateMultiplier(), 0.3*RecoilRateMultiplier());
				AddControllerPitchInput(randF);
				AddControllerYawInput(randF2);
				CanShoot=false;				
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), 1/(BulletsPerSecRifle*FireRateMultiplier()), false);
			}
		}
		else
		{
			if(EmptySoundBoolean==false)
			{
				EmptySoundBoolean=true;
				// 탄약 고갈 사운드 재생
				UGameplayStatics::PlaySound2D(GetWorld(), BulletEmptySound);
			}
		}
	}
	//  스나이퍼를 들고 있는 상태라면
	else if(weaponArray[1]==true)
	{
		if(curSniperAmmo>0)
		{
			// Clamp를 통한 탄약 수 차감
			curSniperAmmo = FMath::Clamp(curSniperAmmo-1, 0, 5+SetSniperAdditionalMagazine());
			UE_LOG(LogTemp, Warning, TEXT("Cur Sniper Bullet : %d"), curSniperAmmo)
			FVector startLoc = FollowCamera->GetComponentLocation();
			FVector EndLoc = startLoc + FollowCamera->GetForwardVector()*10000.0f;
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // LineTrace로 히트 가능한 오브젝트 유형들.
			TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
			TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);
			TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
			TEnumAsByte<EObjectTypeQuery> PhysicsBody = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody);
			TEnumAsByte<EObjectTypeQuery> Destructible = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Destructible);
			ObjectTypes.Add(WorldStatic);
			ObjectTypes.Add(WorldDynamic);
			ObjectTypes.Add(Pawn);
			ObjectTypes.Add(PhysicsBody);
			ObjectTypes.Add(Destructible);
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this); // LineTrace에서 제외할 대상
			FHitResult sniperHitResult;
			FActorSpawnParameters param;
			param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			auto spawnTrans = sniperComp->GetSocketTransform(FName("BulletShell"));
			auto bulletShell = GetWorld()->SpawnActor<AActor>(BulletShellFactory, spawnTrans);
			bulletShell->SetLifeSpan(5.0f);
			auto bulSoundLoc = GetActorLocation()*FVector(0, 0, -80);
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), RifleBulletShellDropSound, bulSoundLoc, FRotator::ZeroRotator, 0.4, 1, 0);
			UGameplayStatics::PlaySound2D(GetWorld(), SniperFireSound);
			bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),startLoc, EndLoc, ObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::None, sniperHitResult, true);
			// 라인 트레이스가 적중했다면
			if(bHit)
			{
				// Enemy Casting
				AEnemy* enemy=Cast<AEnemy>(sniperHitResult.GetActor());
				// Enemy FSM Casting
				UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
				// Reward Container Casting
				ARewardContainer* rewardContainer=Cast<ARewardContainer>(sniperHitResult.GetActor());
				if(fsm&&enemy)
				{
					auto guardian=Cast<AGuardian>(enemy);
					auto crunch = Cast<ACrunch>(enemy);
					if(guardian)
					{
						bGuardian=true;
					}
					else if(crunch)
					{
						bCrunch=true;
					}
					// 이미 죽지 않은 적에게만 실행
					if(enemy->bDeath==false)
					{
						hitActors = sniperHitResult.GetActor();
						auto hitBone = sniperHitResult.BoneName;
						auto hitLoc = sniperHitResult.Location;
						auto hitRot = UKismetMathLibrary::Conv_VectorToRotator(sniperHitResult.ImpactNormal);
						if(hitBone==FName("head"))
						{
							auto min = FMath::RoundFromZero(1000*DamageMultiplier());
							auto max = FMath::RoundFromZero(1200*DamageMultiplier());
							randSniperHeadDamage = FMath::RandRange(min, max);
							if(enemy->curHP<=randSniperHeadDamage)
							{
								crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(3.0f));
								// FSM에 있는 Damage Process 호출		
								fsm->OnDamageProcess(randSniperHeadDamage);
								SetDamageWidget(randSniperHeadDamage, hitLoc, false, FLinearColor::Yellow);
								// 헤드 적중 데미지 프로세스 호출
								enemy->OnHeadDamaged();								
								enemy->bDeath=true;
								if(bGuardian)
								{
									GuardianCount++;
									informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
									enemy->DropReward();
								}
								else if(bCrunch)
								{
									BossCount++;
									informationUI->BossCount->SetText(FText::AsNumber(BossCount));
									SetBossHPWidget(enemy);
									enemy->DropReward();
									FTimerHandle removeHandle;
									GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
								}
							}
							else
							{
								crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHeadHitSound, hitLoc);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(3.0f));
								// 적중 대상이 보스라면
								if(bCrunch)
								{
									// 실드가 파괴된 상태라면
									if(enemy->isShieldBroken)
									{
										// FSM에 있는 Damage Process 호출		
										fsm->OnDamageProcess(randSniperHeadDamage);
										if(enemy->isStunned)
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randSniperHeadDamage*2, hitLoc, false, FLinearColor::Red);
										}
										else
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randSniperHeadDamage, hitLoc, false, FLinearColor::Yellow);
										}										
										// 헤드 적중 데미지 프로세스 호출
										enemy->OnHeadDamaged();
									}
									// 실드가 파괴되지 않은 상태라면
									else
									{
										auto EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
										EmitterTrans.SetScale3D(FVector(1.3));
										UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
										// FSM에 있는 Damage Process 호출		
										fsm->OnShieldDamageProcess(randSniperHeadDamage);
										// 데미지 위젯에 피해 값과 적 위치벡터 할당
										SetDamageWidget(randSniperHeadDamage/20, hitLoc, true, FLinearColor::Yellow);
										// 헤드 적중 데미지 프로세스 호출
										enemy->OnHeadDamaged();
									}
									SetBossHPWidget(enemy);
								}
								// 보스가 아니라면
								else
								{
									// FSM에 있는 Damage Process 호출		
									fsm->OnDamageProcess(randSniperHeadDamage);
									// 데미지 위젯에 피해 값과 적 위치벡터 할당
									SetDamageWidget(randSniperHeadDamage, hitLoc, false, FLinearColor::Yellow);
									// 헤드 적중 데미지 프로세스 호출
									enemy->OnHeadDamaged();
								}
							}
						}
						else
						{
							auto min = FMath::RoundFromZero(650*DamageMultiplier());
							auto max = FMath::RoundFromZero(850*DamageMultiplier());
							randSniperDamage = FMath::RandRange(min, max);
							if(enemy->curHP<=randSniperDamage)
							{
								crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(3.0f));
								// FSM에 있는 Damage Process 호출		
								fsm->OnDamageProcess(randSniperDamage);
								SetDamageWidget(randSniperDamage, hitLoc, false, FLinearColor::White);

								// 일반 적중 데미지 프로세스 호출
								enemy->OnDamaged();
								enemy->bDeath=true;
								if(bGuardian)
								{
									GuardianCount++;
									informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
									enemy->DropReward();
								}
								else if(bCrunch)
								{
									BossCount++;
									informationUI->BossCount->SetText(FText::AsNumber(BossCount));
									SetBossHPWidget(enemy);
									enemy->DropReward();
									FTimerHandle removeHandle;
									GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
								}
							}
							else
							{
								crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, hitLoc);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(0.8f));
								// 적중 대상이 보스라면
								if(bCrunch)
								{
									// 실드가 파괴된 상태라면
									if(enemy->isShieldBroken)
									{
										// FSM에 있는 Damage Process 호출		
										fsm->OnDamageProcess(randSniperDamage);
										if(enemy->isStunned)
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randSniperDamage*2, hitLoc, false, FLinearColor::Red);
										}
										else
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randSniperDamage, hitLoc, false, FLinearColor::White);
										}
										// 일반 적중 데미지 프로세스 호출
										enemy->OnDamaged();
									}
									// 실드가 있는 상태라면
									else
									{
										auto EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
										EmitterTrans.SetScale3D(FVector(1.3));
										UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
										// FSM에 있는 Damage Process 호출		
										fsm->OnShieldDamageProcess(randSniperDamage);
										// 데미지 위젯에 피해 값과 적 위치벡터 할당
										SetDamageWidget(randSniperDamage/20, hitLoc, true, FLinearColor::White);
										// 일반 적중 데미지 프로세스 호출
										enemy->OnDamaged();
									}
									SetBossHPWidget(enemy);
								}
								// 보스가 아니라면
								else
								{
									// FSM에 있는 Damage Process 호출		
									fsm->OnDamageProcess(randSniperDamage);
									// 데미지 위젯에 피해 값과 적 위치벡터 할당
									SetDamageWidget(randSniperDamage, hitLoc, false, FLinearColor::White);
									// 일반 적중 데미지 프로세스 호출
									enemy->OnDamaged();
								}
							}
						}
					}
					bGuardian=false;
					bCrunch=false;
				}
				else if(rewardContainer)
				{
					if(!rewardContainer->bDestroyed)
					{
						auto hitLoc = sniperHitResult.Location;
						crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, hitLoc);
						if(rewardContainer->curBoxHP<=5)
						{
							rewardContainer->BoxDestroyed();
							rewardContainer->containerMesh->SetSimulatePhysics(true);
							ContainerLoc = rewardContainer->GetActorLocation();
							containerDele.ExecuteIfBound();
						}
						else
						{
							rewardContainer->curBoxHP=FMath::Clamp(rewardContainer->curBoxHP-5, 0, 10);
						}
					}
				}
				else
				{					
					auto decalRot = UKismetMathLibrary::Conv_VectorToRotator(sniperHitResult.ImpactNormal);
					auto decalLoc = sniperHitResult.Location;
					auto decalTrans = UKismetMathLibrary::MakeTransform(decalLoc, decalRot);
					GetWorld()->SpawnActor<AActor>(ShotDecalFactory, decalTrans);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, decalLoc, decalRot+FRotator(-90, 0, 0), FVector(0.5f));
				}					
				auto randF = UKismetMathLibrary::RandomFloatInRange(-0.7*RecoilRateMultiplier(), -1.2*RecoilRateMultiplier());
				auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.7*RecoilRateMultiplier(), 0.8*RecoilRateMultiplier());
				AddControllerPitchInput(randF);
				AddControllerYawInput(randF2);				
				if(isSniperZooming)
				{
					auto particleTrans = FollowCamera->GetComponentLocation()+FollowCamera->GetUpVector()*-70.0f;
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SniperFireParticle, particleTrans);
					auto controller = GetWorld()->GetFirstPlayerController();
					controller->PlayerCameraManager->StartCameraShake(sniperCameraShake);
					auto fireSocketLoc = FollowCamera->GetComponentLocation()+FollowCamera->GetUpVector()*-70.0f;
				}
				else
				{
					PlayAnimMontage(zoomingMontage, 1, FName("RifleFire"));
					auto controller = GetWorld()->GetFirstPlayerController();
					controller->PlayerCameraManager->StartCameraShake(sniperFireShake);
					auto particleTrans = sniperComp->GetSocketTransform(FName("SniperFirePosition"));
					particleTrans.SetScale3D(FVector(0.7));
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SniperFireParticle, particleTrans);
				}

				CanShoot=false;
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), BulletsPerSecSniper*FireRateMultiplier(), false);
			}
			// 라인 트레이스가 적중하지 않았다면
			else
			{
				auto randF = UKismetMathLibrary::RandomFloatInRange(-0.7*RecoilRateMultiplier(), -1.2*RecoilRateMultiplier());
				auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.7*RecoilRateMultiplier(), 0.8*RecoilRateMultiplier());
				AddControllerPitchInput(randF);
				AddControllerYawInput(randF2);
				if(isZooming)
				{
					auto particleTrans = FollowCamera->GetComponentLocation()+FollowCamera->GetUpVector()*-70.0f;
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SniperFireParticle, particleTrans);
				}
				else
				{
					auto particleTrans = sniperComp->GetSocketTransform(FName("SniperFirePosition"));
					particleTrans.SetScale3D(FVector(0.7));
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SniperFireParticle, particleTrans);
				}
				CanShoot=false;				
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), BulletsPerSecSniper*FireRateMultiplier(), false);
			}
		}
		else
		{
			if(EmptySoundBoolean==false)
			{
				EmptySoundBoolean=true;
				// 탄약 고갈 사운드 재생
				UGameplayStatics::PlaySound2D(GetWorld(), BulletEmptySound);
			}
		}		
	}
	// 권총을 들고 있는 상태라면
	else if(weaponArray[2]==true)
	{
		if(curPistolAmmo>0)
		{
			// Clamp를 통한 탄약 수 차감
			curPistolAmmo = FMath::Clamp(curPistolAmmo-1, 0, 8+SetPistolAdditionalMagazine());
			UE_LOG(LogTemp, Warning, TEXT("Cur Pistol Bullet : %d"), curPistolAmmo)
			auto controller = GetWorld()->GetFirstPlayerController();
			controller->PlayerCameraManager->StartCameraShake(pistolFireShake);
			if(isZooming)
			{
				PlayAnimMontage(zoomingMontage, 1, FName("PistolZoomFire"));
			}
			else
			{
				PlayAnimMontage(zoomingMontage, 1, FName("PistolFire"));
			}
			FVector startLoc = FollowCamera->GetComponentLocation();
			FVector EndLoc = startLoc + FollowCamera->GetForwardVector()*10000.0f;
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // LineTrace로 히트 가능한 오브젝트 유형들.
			TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
			TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);
			TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
			TEnumAsByte<EObjectTypeQuery> PhysicsBody = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody);
			TEnumAsByte<EObjectTypeQuery> Destructible = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Destructible);
			ObjectTypes.Add(WorldStatic);
			ObjectTypes.Add(WorldDynamic);
			ObjectTypes.Add(Pawn);
			ObjectTypes.Add(PhysicsBody);
			ObjectTypes.Add(Destructible);
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this); // LineTrace에서 제외할 대상
			FHitResult pistolHitResult;
			auto particleTrans = pistolComp->GetSocketTransform(FName("PistolFirePosition"));
			particleTrans.SetScale3D(FVector(0.7));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolfireParticle, particleTrans);
			FActorSpawnParameters param;
			param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			auto spawnTrans = pistolComp->GetSocketTransform(FName("BulletShell"));
			auto bulletShell = GetWorld()->SpawnActor<AActor>(BulletShellFactory, spawnTrans);
			bulletShell->SetLifeSpan(5.0f);
			auto bulSoundLoc = GetActorLocation()*FVector(0, 0, -80);
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), RifleBulletShellDropSound, bulSoundLoc, FRotator::ZeroRotator, 0.4, 1, 0);
			UGameplayStatics::PlaySound2D(GetWorld(), PistolFireSound);
			bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),startLoc, EndLoc, ObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::None, pistolHitResult, true);
			if(bHit)
			{
				// Enemy Casting
				AEnemy* enemy=Cast<AEnemy>(pistolHitResult.GetActor());
				// Enemy FSM Casting
				UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
				// Reward Container Casting
				ARewardContainer* rewardContainer=Cast<ARewardContainer>(pistolHitResult.GetActor());
				if(fsm&&enemy)
				{
					auto guardian=Cast<AGuardian>(enemy);
					auto crunch = Cast<ACrunch>(enemy);
					if(guardian)
					{
						bGuardian=true;
					}
					else if(crunch)
					{
						bCrunch=true;
					}
					// 이미 죽지 않은 적에게만 실행
					if(enemy->bDeath==false)
					{
						hitActors = pistolHitResult.GetActor();
						auto hitBone = pistolHitResult.BoneName;
						auto hitLoc = pistolHitResult.Location;
						auto hitRot = UKismetMathLibrary::Conv_VectorToRotator(pistolHitResult.ImpactNormal);
						if(hitBone==FName("head"))
						{
							auto min = FMath::RoundFromZero(450*DamageMultiplier());
							auto max = FMath::RoundFromZero(550*DamageMultiplier());
							randPistolHeadDamage = FMath::RandRange(min, max);
							// 이번 공격에 Enemy가 죽는다면
							if(enemy->curHP<=randPistolHeadDamage)
							{
								crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.5f));
								// FSM에 있는 Damage Process 호출		
								fsm->OnDamageProcess(randPistolHeadDamage);
								SetDamageWidget(randPistolHeadDamage, hitLoc, false, FLinearColor::Yellow);

								// 헤드 적중 데미지 프로세스 호출
								enemy->OnHeadDamaged();
								enemy->bDeath=true;
								if(bGuardian)
								{
									GuardianCount++;
									informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
									enemy->DropReward();
								}
								else if(bCrunch)
								{
									BossCount++;
									informationUI->BossCount->SetText(FText::AsNumber(BossCount));
									SetBossHPWidget(enemy);
									enemy->DropReward();
									FTimerHandle removeHandle;
									GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
								}
							}
							else
							{
								crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHeadHitSound, hitLoc);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.5f));
								// 적중 대상이 보스라면
								if(bCrunch)
								{
									// 실드가 파괴된 상태라면
									if(enemy->isShieldBroken)
									{
										// FSM에 있는 Damage Process 호출		
										fsm->OnDamageProcess(randPistolHeadDamage);
										if(enemy->isStunned)
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randPistolHeadDamage*2, hitLoc, false, FLinearColor::Red);
										}
										else
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randPistolHeadDamage, hitLoc, false, FLinearColor::Yellow);
										}										
										// 헤드 적중 데미지 프로세스 호출
										enemy->OnHeadDamaged();
									}
									// 실드가 파괴되지 않은 상태라면
									else
									{
										auto EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
										EmitterTrans.SetScale3D(FVector(1.3));
										UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
										// FSM에 있는 Damage Process 호출		
										fsm->OnShieldDamageProcess(randPistolHeadDamage);
										// 데미지 위젯에 피해 값과 적 위치벡터 할당
										SetDamageWidget(randPistolHeadDamage/20, hitLoc, true, FLinearColor::Yellow);
										// 헤드 적중 데미지 프로세스 호출
										enemy->OnHeadDamaged();
									}
									SetBossHPWidget(enemy);
								}
								// 보스가 아니라면
								else
								{
									// FSM에 있는 Damage Process 호출		
									fsm->OnDamageProcess(randPistolHeadDamage);
									// 데미지 위젯에 피해 값과 적 위치벡터 할당
									SetDamageWidget(randPistolHeadDamage, hitLoc, false, FLinearColor::Yellow);
									// 헤드 적중 데미지 프로세스 호출
									enemy->OnHeadDamaged();
								}
							}
						}
						else
						{
							auto min = FMath::RoundFromZero(220*DamageMultiplier());
							auto max = FMath::RoundFromZero(300*DamageMultiplier());
							randPistolDamage = FMath::RandRange(min, max);
							// 이번 공격에 Enemy가 죽는다면
							if(enemy->curHP<=randPistolDamage)
							{
								crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.5f));
								// FSM에 있는 Damage Process 호출		
								fsm->OnDamageProcess(randPistolDamage);
								SetDamageWidget(randPistolDamage, hitLoc, false, FLinearColor::White);

								// 일반 적중 데미지 프로세스 호출
								enemy->OnDamaged();
								enemy->bDeath=true;
								if(bGuardian)
								{
									GuardianCount++;
									informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
									enemy->DropReward();

								}
								else if(bCrunch)
								{
									BossCount++;
									informationUI->BossCount->SetText(FText::AsNumber(BossCount));
									SetBossHPWidget(enemy);
									enemy->DropReward();
									FTimerHandle removeHandle;
									GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
								}
							}
							else
							{
								crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, hitLoc);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(0.7f));
								// 적중 대상이 보스라면
								if(bCrunch)
								{
									// 실드가 파괴된 상태라면
									if(enemy->isShieldBroken)
									{
										// FSM에 있는 Damage Process 호출		
										fsm->OnDamageProcess(randPistolDamage);
										if(enemy->isStunned)
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randPistolDamage*2, hitLoc, false, FLinearColor::Red);
										}
										else
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randPistolDamage, hitLoc, false, FLinearColor::White);
										}
										// 일반 적중 데미지 프로세스 호출
										enemy->OnDamaged();
									}
									// 실드가 있는 상태라면
									else
									{
										auto EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
										EmitterTrans.SetScale3D(FVector(1.3));
										UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
										// FSM에 있는 Damage Process 호출		
										fsm->OnShieldDamageProcess(randPistolDamage);
										// 데미지 위젯에 피해 값과 적 위치벡터 할당
										SetDamageWidget(randPistolDamage/20, hitLoc, true, FLinearColor::White);
										// 일반 적중 데미지 프로세스 호출
										enemy->OnDamaged();
									}
									SetBossHPWidget(enemy);
								}
								// 보스가 아니라면
								else
								{
									// FSM에 있는 Damage Process 호출		
									fsm->OnDamageProcess(randPistolDamage);
									// 데미지 위젯에 피해 값과 적 위치벡터 할당
									SetDamageWidget(randPistolDamage, hitLoc, false, FLinearColor::White);
									// 일반 적중 데미지 프로세스 호출
									enemy->OnDamaged();
								}
							}
						}
					}
					bGuardian=false;
					bCrunch=false;
				}
				else if(rewardContainer)
				{
					if(!rewardContainer->bDestroyed)
					{
						auto hitLoc = pistolHitResult.Location;
						crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, hitLoc);
						if(rewardContainer->curBoxHP<=2)
						{
							rewardContainer->BoxDestroyed();
							rewardContainer->containerMesh->SetSimulatePhysics(true);
							ContainerLoc = rewardContainer->GetActorLocation();
							containerDele.ExecuteIfBound();
						}
						else
						{
							rewardContainer->curBoxHP=FMath::Clamp(rewardContainer->curBoxHP-2, 0, 10);
						}
					}
				}
				else
				{
					FActorSpawnParameters params;
					params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					auto decalRot = UKismetMathLibrary::Conv_VectorToRotator(pistolHitResult.ImpactNormal);
					auto decalLoc = pistolHitResult.Location;
					auto decalTrans = UKismetMathLibrary::MakeTransform(decalLoc, decalRot);
					GetWorld()->SpawnActor<AActor>(ShotDecalFactory, decalTrans);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, decalLoc, decalRot+FRotator(-90, 0, 0), FVector(0.5f));
				}
				auto randF = UKismetMathLibrary::RandomFloatInRange(-0.7*RecoilRateMultiplier(), -1.2*RecoilRateMultiplier());
				auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.7*RecoilRateMultiplier(), 0.8*RecoilRateMultiplier());
				AddControllerPitchInput(randF);
				AddControllerYawInput(randF2);				
				auto fireSocketLoc = pistolComp->GetSocketTransform(FName("PistolFirePosition")).GetLocation();
				// 탄 궤적 나이아가라 시스템 스폰
				// UNiagaraComponent* niagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTrailSystem, pistolHitResult.Location, FRotator::ZeroRotator,FVector(1), true, true, ENCPoolMethod::AutoRelease);
				// if(niagara)
				// {
				// 	// 나이아가라 파라미터 벡터 위치 변수 할당
				// 	niagara->SetVectorParameter(FName("EndPoint"), fireSocketLoc);
				// }
				CanShoot=false;
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), 1/(BulletsPerSecPistol*FireRateMultiplier()), false);
			}
			else
			{
				auto randF = UKismetMathLibrary::RandomFloatInRange(-0.7*RecoilRateMultiplier(), -1.2*RecoilRateMultiplier());
				auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.7*RecoilRateMultiplier(), 0.8*RecoilRateMultiplier());
				AddControllerPitchInput(randF);
				AddControllerYawInput(randF2);
				FVector niagaraSpawnLoc = FollowCamera->K2_GetComponentLocation();
				FVector ForwardLoc = niagaraSpawnLoc + FollowCamera->GetForwardVector()*10000.0f;
				auto FireLoc = pistolComp->GetSocketTransform(FName("PistolFirePosition")).GetLocation();
				// UNiagaraComponent* niagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTrailSystem, ForwardLoc, FRotator::ZeroRotator, FVector(1), true, true, ENCPoolMethod::AutoRelease);
				// if(niagara)
				// {
				// 	niagara->SetVectorParameter(FName("EndPoint"), FireLoc);
				// }
				CanShoot=false;				
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), 1/(BulletsPerSecPistol*FireRateMultiplier()), false);
			}
		}
		else
		{
			if(EmptySoundBoolean==false)
			{
				EmptySoundBoolean=true;
				// 탄약 고갈 사운드 재생
				UGameplayStatics::PlaySound2D(GetWorld(), BulletEmptySound);
			}
		}
	}
	// M249를 들고 있으면서 사격 가능한 상태라면
	if(weaponArray[3]==true)
	{
		if(curM249Ammo>0)
		{
			// Clamp를 통한 탄약 수 차감
			curM249Ammo = FMath::Clamp(curM249Ammo-1, 0, 100+SetM249AdditionalMagazine());
			UE_LOG(LogTemp, Warning, TEXT("Cur M249 Bullet : %d"), curM249Ammo)
			FVector startLoc = FollowCamera->GetComponentLocation();
			FVector EndLoc = startLoc + FollowCamera->GetForwardVector()*10000.0f;
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // LineTrace로 히트 가능한 오브젝트 유형들.
			TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
			TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);
			TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
			TEnumAsByte<EObjectTypeQuery> PhysicsBody = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody);
			TEnumAsByte<EObjectTypeQuery> Destructible = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Destructible);
			ObjectTypes.Add(WorldStatic);
			ObjectTypes.Add(WorldDynamic);
			ObjectTypes.Add(Pawn);
			ObjectTypes.Add(PhysicsBody);
			ObjectTypes.Add(Destructible);
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this); // LineTrace에서 제외할 대상
			FHitResult M249HitResult;
			auto particleTrans = m249Comp->GetSocketTransform(FName("M249FirePosition"));
			particleTrans.SetScale3D(FVector(0.7));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PistolfireParticle, particleTrans);
			FActorSpawnParameters param;
			param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			auto spawnTrans = m249Comp->GetSocketTransform(FName("BulletShell"));
			auto bulletShell = GetWorld()->SpawnActor<AActor>(BulletShellFactory, spawnTrans);
			bulletShell->SetLifeSpan(5.0f);
			auto bulSoundLoc = GetActorLocation()*FVector(0, 0, -80);
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), RifleBulletShellDropSound, bulSoundLoc, FRotator::ZeroRotator, 0.4, 1, 0);
			UGameplayStatics::PlaySound2D(GetWorld(), M249FireSound);
			auto controller = GetWorld()->GetFirstPlayerController();
			controller->PlayerCameraManager->StartCameraShake(rifleFireShake);
			PlayAnimMontage(zoomingMontage, 1, FName("RifleFire"));
			bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),startLoc, EndLoc, ObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::None, M249HitResult, true);
			if(bHit)
			{
				// Enemy Casting
				AEnemy* enemy=Cast<AEnemy>(M249HitResult.GetActor());
				// Enemy FSM Casting
				UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(FName("enemyFSM")));
				// Reward Container Casting
				ARewardContainer* rewardContainer=Cast<ARewardContainer>(M249HitResult.GetActor());
				if(fsm&&enemy)
				{
					auto guardian=Cast<AGuardian>(enemy);
					auto crunch = Cast<ACrunch>(enemy);
					if(guardian)
					{
						bGuardian=true;
					}
					else if(crunch)
					{
						bCrunch=true;
					}
					// 이미 죽지 않은 적에게만 실행
					if(enemy->bDeath==false)
					{
						hitActors = M249HitResult.GetActor();
						auto hitBone = M249HitResult.BoneName;
						auto hitLoc = M249HitResult.Location;
						auto hitRot = UKismetMathLibrary::Conv_VectorToRotator(M249HitResult.ImpactNormal);
						if(hitBone==FName("head"))
						{
							auto min = FMath::RoundFromZero(180*DamageMultiplier());
							auto max = FMath::RoundFromZero(220*DamageMultiplier());
							randM249HeadDamage = FMath::RandRange(min, max);
							// 이번 공격에 Enemy가 죽는다면
							if(enemy->curHP<=randM249HeadDamage)
							{
								crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.0f));
								// FSM에 있는 Damage Process 호출		
								fsm->OnDamageProcess(randM249HeadDamage);
								SetDamageWidget(randM249HeadDamage, hitLoc, false, FLinearColor::Yellow);

								// 헤드 적중 데미지 프로세스 호출
								enemy->OnHeadDamaged();
								enemy->bDeath=true;
								if(bGuardian)
								{
									GuardianCount++;
									enemy->DropReward();
									informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
								}
								else if(bCrunch)
								{
									BossCount++;
									informationUI->BossCount->SetText(FText::AsNumber(BossCount));
									SetBossHPWidget(enemy);
									enemy->DropReward();
									FTimerHandle removeHandle;
									GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
								}
							}
							else
							{
								crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation);
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHeadHitSound, hitLoc);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.0f));
								// 적중 대상이 보스라면
								if(bCrunch)
								{
									// 실드가 파괴된 상태라면
									if(enemy->isShieldBroken)
									{
										// FSM에 있는 Damage Process 호출		
										fsm->OnDamageProcess(randM249HeadDamage);
										if(enemy->isStunned)
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randM249HeadDamage*2, hitLoc, false, FLinearColor::Red);
										}
										else
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randM249HeadDamage, hitLoc, false, FLinearColor::Yellow);
										}										
										// 헤드 적중 데미지 프로세스 호출
										enemy->OnHeadDamaged();
									}
									// 실드가 파괴되지 않은 상태라면
									else
									{
										auto EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
										EmitterTrans.SetScale3D(FVector(1.3));
										UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
										// FSM에 있는 Damage Process 호출		
										fsm->OnShieldDamageProcess(randM249HeadDamage);
										// 데미지 위젯에 피해 값과 적 위치벡터 할당
										SetDamageWidget(randM249HeadDamage/20, hitLoc, true, FLinearColor::Yellow);
										// 헤드 적중 데미지 프로세스 호출
										enemy->OnHeadDamaged();
									}
									SetBossHPWidget(enemy);
								}
								// 보스가 아니라면
								else
								{
									// FSM에 있는 Damage Process 호출		
									fsm->OnDamageProcess(randM249HeadDamage);
									// 데미지 위젯에 피해 값과 적 위치벡터 할당
									SetDamageWidget(randM249HeadDamage, hitLoc, false, FLinearColor::Yellow);
									// 헤드 적중 데미지 프로세스 호출
									enemy->OnHeadDamaged();
								}
							}
						}
						else
						{
							auto min = FMath::RoundFromZero(90*DamageMultiplier());
							auto max = FMath::RoundFromZero(110*DamageMultiplier());
							randM249Damage = FMath::RandRange(min, max);
							// 이번 공격에 Enemy가 죽는다면
							if(enemy->curHP<=randM249Damage)
							{
								crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation);
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), KillSound, hitLoc);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(2.0f));
								// FSM에 있는 Damage Process 호출		
								fsm->OnDamageProcess(randM249Damage);
								SetDamageWidget(randM249Damage, hitLoc, false, FLinearColor::White);

								// 일반 적중 데미지 프로세스 호출
								enemy->OnDamaged();
								enemy->bDeath=true;
								if(bGuardian)
								{
									GuardianCount++;
									enemy->DropReward();
									informationUI->GuardianCount->SetText(FText::AsNumber(GuardianCount));
								}
								else if(bCrunch)
								{
									BossCount++;
									informationUI->BossCount->SetText(FText::AsNumber(BossCount));
									SetBossHPWidget(enemy);
									enemy->DropReward();
									FTimerHandle removeHandle;
									GetWorldTimerManager().SetTimer(removeHandle, this, &APlayerCharacter::RemoveBossHPWidget, 4.0f, false);
								}
							}
							else
							{
								crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
								UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, hitLoc);
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, hitLoc, hitRot, FVector(0.5f));
								// 적중 대상이 보스라면
								if(bCrunch)
								{
									// 실드가 파괴된 상태라면
									if(enemy->isShieldBroken)
									{
										// FSM에 있는 Damage Process 호출		
										fsm->OnDamageProcess(randM249Damage);
										if(enemy->isStunned)
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randM249Damage*2, hitLoc, false, FLinearColor::Red);
										}
										else
										{
											// 데미지 위젯에 피해 값과 적 위치벡터 할당
											SetDamageWidget(randM249Damage, hitLoc, false, FLinearColor::White);
										}
										// 일반 적중 데미지 프로세스 호출
										enemy->OnDamaged();
									}
									// 실드가 있는 상태라면
									else
									{
										auto EmitterTrans = enemy->GetMesh()->GetSocketTransform(FName("ShieldSocket"));
										EmitterTrans.SetScale3D(FVector(1.3));
										UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldHitEmitter, EmitterTrans);
										// FSM에 있는 Damage Process 호출		
										fsm->OnShieldDamageProcess(randM249Damage);
										// 데미지 위젯에 피해 값과 적 위치벡터 할당
										SetDamageWidget(randM249Damage/20, hitLoc, true, FLinearColor::White);
										// 일반 적중 데미지 프로세스 호출
										enemy->OnDamaged();
									}
									SetBossHPWidget(enemy);
								}
								// 보스가 아니라면
								else
								{
									// FSM에 있는 Damage Process 호출		
									fsm->OnDamageProcess(randM249Damage);
									// 데미지 위젯에 피해 값과 적 위치벡터 할당
									SetDamageWidget(randM249Damage, hitLoc, false, FLinearColor::White);
									// 일반 적중 데미지 프로세스 호출
									enemy->OnDamaged();
								}
							}
						}
					}
					bGuardian=false;
					bCrunch=false;
				}
				else if(rewardContainer)
				{
					if(!rewardContainer->bDestroyed)
					{
						auto hitLoc = M249HitResult.Location;
						crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, hitLoc);
						if(rewardContainer->curBoxHP<=1)
						{
							rewardContainer->BoxDestroyed();
							rewardContainer->containerMesh->SetSimulatePhysics(true);
							ContainerLoc = rewardContainer->GetActorLocation();
							containerDele.ExecuteIfBound();
						}
						else
						{
							rewardContainer->curBoxHP=FMath::Clamp(rewardContainer->curBoxHP-1, 0, 10);
						}
					}
				}
				else
				{
					FActorSpawnParameters params;
					params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					auto decalRot = UKismetMathLibrary::Conv_VectorToRotator(M249HitResult.ImpactNormal);
					auto decalLoc = M249HitResult.Location;
					auto decalTrans = UKismetMathLibrary::MakeTransform(decalLoc, decalRot);
					GetWorld()->SpawnActor<AActor>(ShotDecalFactory, decalTrans);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, decalLoc, decalRot+FRotator(-90, 0, 0), FVector(0.5f));
				}
				if(isZooming)
				{
					auto randF = UKismetMathLibrary::RandomFloatInRange(-0.4*RecoilRateMultiplier(), -0.7*RecoilRateMultiplier());
					auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.4*RecoilRateMultiplier(), 0.4*RecoilRateMultiplier());
					AddControllerPitchInput(randF);
					AddControllerYawInput(randF2);			
				}
				else
				{
					auto randF = UKismetMathLibrary::RandomFloatInRange(-0.6*RecoilRateMultiplier(), -1.1*RecoilRateMultiplier());
					auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.5*RecoilRateMultiplier(), 0.5*RecoilRateMultiplier());
					AddControllerPitchInput(randF);
					AddControllerYawInput(randF2);					
				}				
				CanShoot=false;
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), 1/(BulletsPerSecM249*FireRateMultiplier()), false);
			}
			else
			{
				if(isZooming)
				{
					auto randF = UKismetMathLibrary::RandomFloatInRange(-0.4*RecoilRateMultiplier(), -0.7*RecoilRateMultiplier());
					auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.4*RecoilRateMultiplier(), 0.4*RecoilRateMultiplier());
					AddControllerPitchInput(randF);
					AddControllerYawInput(randF2);			
				}
				else
				{
					auto randF = UKismetMathLibrary::RandomFloatInRange(-0.6*RecoilRateMultiplier(), -1.1*RecoilRateMultiplier());
					auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.5*RecoilRateMultiplier(), 0.5*RecoilRateMultiplier());
					AddControllerPitchInput(randF);
					AddControllerYawInput(randF2);					
				}
				CanShoot=false;				
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), 1/(BulletsPerSecM249*FireRateMultiplier()), false);
			}
		}
		else
		{
			if(EmptySoundBoolean==false)
			{
				EmptySoundBoolean=true;
				// 탄약 고갈 사운드 재생
				UGameplayStatics::PlaySound2D(GetWorld(), BulletEmptySound);
			}
		}
	}
}

void APlayerCharacter::FireRelease()
{
	EmptySoundBoolean=false;
}


void APlayerCharacter::RemoveBossHPWidget()
{
	if(bossHPUI)
	{
		bossHPUI->RemoveFromParent();
	}
}

void APlayerCharacter::InfoWidgetUpdate()
{
}

float APlayerCharacter::DamageMultiplier()
{
	if(HeadsetEquipped)
	{
		return 1.16f;
	}
	return 1.f;
}

float APlayerCharacter::FireRateMultiplier()
{
	if(MaskEquipped)
	{
		return 1.23f;
	}
	return 1.f;
}

float APlayerCharacter::RecoilRateMultiplier()
{
	if(GoggleEquipped)
	{
		return 1.2f;
	}
	return 1.f;
}

void APlayerCharacter::PlayerDeath()
{
	if(isDead==false)
	{
		// 사망지점 전역변수에 캐싱
		DeathPosition=GetActorLocation();
		auto playerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		// 카메라 페이드 연출
		playerCam->StartCameraFade(0, 1, 5.0, FLinearColor::Black, false, true);
		// 몽타주 재생 중단
		StopAnimMontage();
		// 인풋 비활성화
		DisableInput(GetWorld()->GetFirstPlayerController());
		// 사망 몽타주 재생
		PlayAnimMontage(zoomingMontage, 1, FName("Death"));
		infoWidgetUI->RemoveFromParent();
		informationUI->RemoveFromParent();
		crosshairUI->RemoveFromParent();
		FTimerHandle endHandle;
		// 7초 뒤 호출되는 함수 타이머
		GetWorldTimerManager().SetTimer(endHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			// 사망 변수 활성화
			bPlayerDeath=true;
			auto* PC = Cast<AEclipsePlayerController>(GetController());
			// 현재 주요 변수 값들을 GameInstance의 변수에 캐싱
			CachingValues();
			PouchCaching();
			StashCaching();
			GearCaching();
			MagCaching();
			ClearInventoryCache();
			// 자신 제거
			this->Destroy();
			// 컨트롤러의 리스폰 함수 호출
			PC->Respawn(this);	
		}), 7.0f, false);
		FTimerHandle possesHandle;
		// 0.4초 뒤 호출되는 함수 타이머
		GetWorld()->GetTimerManager().SetTimer(possesHandle, FTimerDelegate::CreateLambda([this]()->void
			{
				auto* PC = Cast<AEclipsePlayerController>(GetController());
				if (PC != nullptr)
				{
					// 리스폰 된 플레이어에 새롭게 빙의
					PC->Possess(this);
				}
			}), 0.4f, false);
		
		isDead=true;
	}
}

void APlayerCharacter::EquipHelmet(bool SoundBool)
{
	if(SoundBool)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), gearEquipSound);
	}
	HelmetSlot->SetVisibility(true);
	HelmetEquipped=true;
}

void APlayerCharacter::EquipHeadset(bool SoundBool)
{
	if(SoundBool)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), gearEquipSound);
	}
	HeadSetSlot->SetVisibility(true);
	HeadsetEquipped=true;
}

void APlayerCharacter::EquipMask(bool SoundBool)
{
	if(SoundBool)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), gearEquipSound);
	}
	MaskSlot->SetVisibility(true);
	MaskEquipped=true;
}

void APlayerCharacter::EquipGoggle(bool SoundBool)
{
	if(SoundBool)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), gearEquipSound);
	}
	GoggleSlot->SetVisibility(true);
	GoggleEquipped=true;
}

void APlayerCharacter::EquipArmor(bool SoundBool)
{
	if(SoundBool)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), gearEquipSound);
	}
	ArmorSlot->SetVisibility(true);
	ArmorEquipped=true;
	curHP=FMath::Clamp(curHP+35, 0, 135);
	maxHP=FMath::Clamp(maxHP+35, 0, 135);
}

void APlayerCharacter::UnEquipHelmet(bool SoundBool)
{
	if(SoundBool)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), gearUnequipSound);
	}
	HelmetSlot->SetVisibility(false);
	HelmetEquipped=false;
}

void APlayerCharacter::UnEquipHeadset(bool SoundBool)
{
	if(SoundBool)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), gearUnequipSound);
	}	HeadSetSlot->SetVisibility(false);
	HeadsetEquipped=false;
}

void APlayerCharacter::UnEquipMask(bool SoundBool)
{
	if(SoundBool)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), gearUnequipSound);
	}	MaskSlot->SetVisibility(false);
	MaskEquipped=false;
}

void APlayerCharacter::UnEquipGoggle(bool SoundBool)
{
	if(SoundBool)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), gearUnequipSound);
	}	GoggleSlot->SetVisibility(false);
	GoggleEquipped=false;
}


void APlayerCharacter::UnEquipArmor(bool SoundBool)
{
	if(SoundBool)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), gearUnequipSound);
	}	ArmorSlot->SetVisibility(false);
	ArmorEquipped=false;
}
