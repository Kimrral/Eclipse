// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Eclipse/UI/CrosshairWidget.h"
#include "Eclipse/UI/DamageWidget.h"
#include "Eclipse/UI/DamageWidgetActor.h"
#include "Eclipse/Game/EclipseGameInstance.h"
#include "Eclipse/Player/EclipsePlayerController.h"
#include "Eclipse/Enemy/Enemy.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LineTraceDetectionComponent.h"
#include "PlayerInputComponent.h"
#include "Eclipse/Item/GoggleActor.h"
#include "Eclipse/UI/InformationWidget.h"
#include "Eclipse/UI/LevelSelection.h"
#include "Eclipse/Weapon/M249Actor.h"
#include "Eclipse/Prop/MissionChecker.h"
#include "Eclipse/Animation/PlayerAnim.h"
#include "Eclipse/Weapon/RifleActor.h"
#include "Eclipse/Weapon/SniperActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Eclipse/Weapon/PistolActor.h"
#include "Eclipse/Prop/QuitGameActor.h"
#include "Eclipse/UI/QuitWidget.h"
#include "Eclipse/Prop/RewardContainer.h"
#include "Eclipse/Prop/StageBoard.h"
#include "Eclipse/Prop/Stash.h"
#include "Eclipse/UI/WeaponInfoWidget.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/Image.h"
#include "Components/SpotLightComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Eclipse/Animation/FirstPersonPlayerAnim.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/Game/EclipseGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Eclipse/CharacterStat/PlayerCharacterStatComponent.h"
#include "Eclipse/Enemy/Boss.h"
#include "Eclipse/Game/EclipsePlayerState.h"
#include "Eclipse/Prop/DeadPlayerContainer.h"
#include "Eclipse/Prop/Trader.h"
#include "Eclipse/UI/ExtractionCountdown.h"
#include "Eclipse/UI/GuideScriptWidget.h"
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
	GetCharacterMovement()->MaxWalkSpeed = CharacterDefaultWalkSpeed;
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

	FlashLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashLight"));
	FlashLight->SetupAttachment(RifleComp, FName("Flashlight"));
	FlashLight->SetVisibility(false);

	// Stat Component 
	Stat = CreateDefaultSubobject<UPlayerCharacterStatComponent>(TEXT("Stat"));

	// Line Trace Detection Component
	Detection = CreateDefaultSubobject<ULineTraceDetectionComponent>(TEXT("LineTraceDetection"));

	// Create and attach the player input component
	PlayerInputComponentRef = CreateDefaultSubobject<UPlayerInputComponent>(TEXT("PlayerInputComponent"));

	SetReplicates(true);
	NetUpdateFrequency = 200.f;
	MinNetUpdateFrequency = 10.f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->HideBoneByName(TEXT("bot_hand"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(TEXT("shotgun_base"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(TEXT("sniper_can_arm_01"), EPhysBodyOp::PBO_None);

	FirstPersonRifleComp->SetVisibility(false);
	FirstPersonPistolComp->SetVisibility(false);
	FirstPersonCharacterMesh->SetVisibility(false);
	FirstPersonCamera->SetActive(false);
	FollowCamera->SetActive(true);

	// Widget Settings
	crosshairUI = CreateWidget<UCrosshairWidget>(GetWorld(), crosshairFactory);
	quitWidgetUI = CreateWidget<UQuitWidget>(GetWorld(), quitWidgetFactory);
	infoWidgetUI = CreateWidget<UWeaponInfoWidget>(GetWorld(), infoWidgetFactory);
	sniperScopeUI = CreateWidget<UUserWidget>(GetWorld(), sniperScopeFactory);
	damageWidgetUI = CreateWidget<UDamageWidget>(GetWorld(), damageWidgetUIFactory);
	informationUI = CreateWidget<UInformationWidget>(GetWorld(), informationWidgetFactory);
	levelSelectionUI = CreateWidget<ULevelSelection>(GetWorld(), levelSelectionWidgetFactory);
	ExtractionCountdownUI = CreateWidget<UExtractionCountdown>(GetWorld(), ExtractionCountdownWidgetFactory);
	MenuWidgetUI = CreateWidget<UMenuWidget>(GetWorld(), MenuWidgetFactory);
	TradeWidgetUI = CreateWidget<UTradeWidget>(GetWorld(), TradeWidgetFactory);
	GuideScriptWidgetUI = CreateWidget<UGuideScriptWidget>(GetWorld(), GuideScriptWidgetFactory);

	// Casting
	gi = Cast<UEclipseGameInstance>(GetGameInstance());
	PC = Cast<AEclipsePlayerController>(gi->GetFirstLocalPlayerController());
	gm = Cast<AEclipseGameMode>(GetWorld()->GetAuthGameMode());
	animInstance = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());

	gi->IsWidgetOn = false;

	//Add Input Mapping Context
	if (PC && IsLocallyControlled())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			const FInputModeGameOnly InputModeData;
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
	// Tilting Timeline Binding
	if (TiltingCurveFloat)
	{
		FOnTimelineFloat TiltLeftTimelineProgress;
		FOnTimelineFloat TiltRightTimelineProgress;
		TiltLeftTimelineProgress.BindDynamic(this, &APlayerCharacter::SetTiltingLeftValue);
		TiltRightTimelineProgress.BindDynamic(this, &APlayerCharacter::SetTiltingRightValue);
		TiltingLeftTimeline.AddInterpFloat(TiltingCurveFloat, TiltLeftTimelineProgress);
		TiltingRightTimeline.AddInterpFloat(TiltingCurveFloat, TiltRightTimelineProgress);
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PlayerSpawnEmitter, GetActorLocation());

	if (IsLocallyControlled())
	{
		TradeWidgetUI->Construction(this);

		if (APlayerCameraManager* const CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)))
		{
			CameraManager->StopCameraFade();
			CameraManager->StartCameraFade(1.0, 0, 20.0, FColor::Black, false, false);
		}


		// Spawn Player Emitter
		UGameplayStatics::PlaySound2D(GetWorld(), PlayerSpawnSound, 0.6, 1, 0.25);

		const FName IntersectionUnload = FName("Deserted_Road");
		const FName SpacecraftUnload = FName("Map_BigStarStation");
		const FName CallBackFunctionName = FName("WidgetConstruction");
		FLatentActionInfo IntersectionLatentInfo;
		const FLatentActionInfo SpacecraftLatentInfo;
		IntersectionLatentInfo.CallbackTarget = this;
		IntersectionLatentInfo.Linkage = 0;
		IntersectionLatentInfo.UUID = 0;
		IntersectionLatentInfo.ExecutionFunction = CallBackFunctionName;
		UGameplayStatics::UnloadStreamLevel(this, SpacecraftUnload, SpacecraftLatentInfo, false);
		UGameplayStatics::UnloadStreamLevel(this, IntersectionUnload, IntersectionLatentInfo, false);
	}

	bUsingRifle = true;
	bUsingSniper = false;
	bUsingPistol = false;
	bUsingM249 = false;

	equippedWeaponStringArray.Empty();
	equippedWeaponStringArray.Add(FString("Rifle")); //0
	equippedWeaponStringArray.Add(FString("Pistol")); //1

	// Weapon Visibility Settings
	RifleComp->SetVisibility(true);
	SniperComp->SetVisibility(false);
	PistolComp->SetVisibility(false);
	M249Comp->SetVisibility(false);

	// Set Weapon Array
	weaponArray.Add(bUsingRifle); //0
	weaponArray.Add(bUsingSniper); //1
	weaponArray.Add(bUsingPistol); //2
	weaponArray.Add(bUsingM249); //3		

	// Delegate Binding
	ExtractionCountdownUI->ExtractionSuccessDele.AddUObject(this, &APlayerCharacter::ExtractionSuccess);
	Stat->OnHpZero.AddUObject(this, &APlayerCharacter::PlayerDeath);

	// Update Tab Widget Before Widget Constructor
	UpdateTabWidget();

	FTimerHandle ScriptHandle;
	GetWorldTimerManager().SetTimer(ScriptHandle, FTimerDelegate::CreateLambda([&]
	{
		if (IsValid(GuideScriptWidgetUI) && IsValid(PC) && IsLocallyControlled())
		{
			DisableInput(PC);
			GuideScriptWidgetUI->Player = this;
			GuideScriptWidgetUI->AddToViewport();
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, GuideScriptWidgetUI, EMouseLockMode::DoNotLock, false, true);
			PC->SetShowMouseCursor(true);
		}
	}), 2.f, false);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APlayerCharacter::SetPlayerControlRotation_Implementation(const FRotator& DesiredRotation)
{
	if (PC)
	{
		PC->SetControlRotation(DesiredRotation);
	}
}

void APlayerCharacter::WidgetConstruction()
{
	if (IsLocallyControlled())
	{
		if (informationUI && crosshairUI)
		{
			crosshairUI->AddToViewport();
			informationUI->owner = this;
			informationUI->UpdateAmmo();
			informationUI->UpdateAmmo_Secondary();
			informationUI->AddToViewport();
			informationUI->EnterHideout();
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timeline.TickTimeline(DeltaTime);
	if (FirstPersonCharacterMesh->IsVisible())
	{
		TiltingLeftTimeline.TickTimeline(DeltaTime);
		TiltingRightTimeline.TickTimeline(DeltaTime);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		PlayerInputComponentRef->SetupInputBindings(EnhancedInputComponent);
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
		const FRotator HitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		if (IsHeadshot)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), BulletHeadHitSound);
			// 적중 위젯 애니메이션 재생
			crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
			// 데미지 위젯에 피해 값과 적 위치벡터 할당
			SetDamageWidget(DamageAmount * 2, HitResult.Location, false, FLinearColor::Yellow);
			// 적중 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticle, HitResult.Location, HitRot, FVector(1.f));
		}
		else
		{
			UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
			// 적중 위젯 애니메이션 재생
			crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
			// 데미지 위젯에 피해 값과 적 위치벡터 할당
			SetDamageWidget(DamageAmount, HitResult.Location, false, FLinearColor::White);
			// 적중 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticle, HitResult.Location, HitRot, FVector(1.f));
		}
	}
	else
	{
		const FRotator HitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		if (IsHeadshot)
		{
			// 적중 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticle, HitResult.Location, HitRot, FVector(1.7f));
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHeadHitSound, HitResult.Location);
		}
		else
		{
			// 적중 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticle, HitResult.Location, HitRot, FVector(1.f));
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, HitResult.Location);
		}
	}
}

// 플레이어가 다른 플레이어를 처치했을 때 호출되는 함수
void APlayerCharacter::OnPlayerKill()
{
	OnPlayerKillRPCServer();
}

// 서버에서 플레이어 처치를 처리하는 RPC 함수의 구현 부분
void APlayerCharacter::OnPlayerKillRPCServer_Implementation()
{
	OnPlayerKillRPCClient();
}

// 서버에서 플레이어 처치를 처리하는 RPC 함수의 유효성 검사 함수
bool APlayerCharacter::OnPlayerKillRPCServer_Validate()
{
	return true;
}

// 클라이언트에서 플레이어 처치를 처리하는 RPC 함수의 구현 부분
void APlayerCharacter::OnPlayerKillRPCClient_Implementation()
{
	// 크로스헤어 UI에서 처치 애니메이션을 재생
	crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
	// 처치 사운드를 재생
	UGameplayStatics::PlaySound2D(GetWorld(), PlayerKillSound, 1, 1, 0.25);
}

// 적을 맞혔을 때 호출되는 함수
void APlayerCharacter::OnEnemyHit(const FHitResult& HitResult, AEnemy* HitEnemy, bool IsHeadshot)
{
	// 적의 체력이 0보다 클 때만 서버 RPC 호출
	if (HitEnemy->EnemyStat->GetCurrentHp() > 0)
	{
		OnEnemyHitRPCServer(HitResult, HitEnemy, IsHeadshot);
	}
}

// 적을 처치했을 때 호출되는 함수
void APlayerCharacter::OnEnemyKill()
{
	OnEnemyKillRPCServer();
}

// 서버에서 적 처치를 처리하는 RPC 함수의 구현 부분
void APlayerCharacter::OnEnemyKillRPCServer_Implementation()
{
	// 무기에 따라 탄약을 증가시킴
	if (weaponArray[0]) maxRifleAmmo += 20;
	else if (weaponArray[1]) maxSniperAmmo += 4;
	else if (weaponArray[2]) maxPistolAmmo += 6;
	else if (weaponArray[3]) maxM249Ammo += 30;

	OnEnemyKillRPCClient();
}

// 서버에서 적 처치를 처리하는 RPC 함수의 유효성 검사 함수
bool APlayerCharacter::OnEnemyKillRPCServer_Validate()
{
	return true;
}

// 클라이언트에서 적 처치를 처리하는 RPC 함수의 구현 부분
void APlayerCharacter::OnEnemyKillRPCClient_Implementation()
{
	// 크로스헤어 UI에서 처치 애니메이션을 재생
	crosshairUI->PlayAnimation(crosshairUI->KillAppearAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
	// 처치 사운드를 재생
	UGameplayStatics::PlaySound2D(GetWorld(), KillSound);
	// 정보 UI 업데이트
	informationUI->ChargeAmmunitionInfoWidget();
	informationUI->PlayAnimation(informationUI->ChargeAmmunition, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
	informationUI->UpdateAmmo_Secondary();
	// 탄약 픽업 사운드를 일정 시간 후 재생
	FTimerHandle AmmoPickHandle;
	GetWorldTimerManager().SetTimer(AmmoPickHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		UGameplayStatics::PlaySound2D(GetWorld(), AmmoPickupSound);
	}), 1.f, false);
}

// 서버에서 적을 맞혔을 때 호출되는 RPC 함수의 구현 부분
void APlayerCharacter::OnEnemyHitRPCServer_Implementation(const FHitResult& HitResult, AEnemy* HitEnemy, bool IsHeadshot)
{
	OnEnemyHitRPCMulticast(HitResult, HitEnemy, IsHeadshot);
}

// 서버에서 적을 맞혔을 때 호출되는 RPC 함수의 유효성 검사 함수
bool APlayerCharacter::OnEnemyHitRPCServer_Validate(const FHitResult& HitResult, AEnemy* HitEnemy, bool IsHeadshot)
{
	return true;
}

// 멀티캐스트 RPC: 모든 클라이언트에서 적을 맞혔을 때 호출되는 함수
void APlayerCharacter::OnEnemyHitRPCMulticast_Implementation(const FHitResult& HitResult, AEnemy* HitEnemy, const bool IsHeadshot)
{
	if (HasAuthority())
	{
		if (IsHeadshot)
		{
			// 헤드샷일 때 데미지 계산 및 처리
			DamageAmount = Stat->GetAttackDamage(weaponArray, false);
			HitEnemy->Damaged(DamageAmount * 2, this);
			if (const auto HitBoss = Cast<ABoss>(HitEnemy); ::IsValid(HitBoss))
			{
				Stat->AccumulatedDamageToBoss += DamageAmount * 2;
			}
			else
			{
				Stat->AccumulatedDamageToEnemy += DamageAmount * 2;
			}
		}
		else
		{
			// 일반 적중일 때 데미지 계산 및 처리
			DamageAmount = Stat->GetAttackDamage(weaponArray, false);
			HitEnemy->Damaged(DamageAmount, this);
			if (const auto HitBoss = Cast<ABoss>(HitEnemy); ::IsValid(HitBoss))
			{
				Stat->AccumulatedDamageToBoss += DamageAmount;
			}
			else
			{
				Stat->AccumulatedDamageToEnemy += DamageAmount;
			}
		}
	}
	else
	{
		// 클라이언트에서 적을 맞혔을 때 효과 처리
		if (IsLocallyControlled())
		{
			const FRotator HitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
			if (HitEnemy->EnemyStat->IsStunned)
			{
				if (IsHeadshot)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), BulletHeadHitSound);
					// 적중 UI 애니메이션 재생
					crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
					// 데미지 위젯 설정 및 파티클 생성
					SetDamageWidget(DamageAmount * 4, HitResult.Location, false, FLinearColor::Red);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(2.f));
				}
				else
				{
					UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
					// 적중 UI 애니메이션 재생
					crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
					// 데미지 위젯 설정 및 파티클 생성
					SetDamageWidget(DamageAmount * 2, HitResult.Location, false, FLinearColor::Red);
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
						// 적중 UI 애니메이션 재생
						crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
						// 데미지 위젯 설정 및 파티클 생성
						SetDamageWidget(DamageAmount * 2, HitResult.Location, false, FLinearColor::Yellow);
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(2.f));
					}
					else
					{
						UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
						// 적중 UI 애니메이션 재생
						crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
						// 데미지 위젯 설정 및 파티클 생성
						SetDamageWidget(DamageAmount, HitResult.Location, false, FLinearColor::White);
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(1.f));
					}
				}
				else
				{
					if (IsHeadshot)
					{
						UGameplayStatics::PlaySound2D(GetWorld(), BulletHeadHitSound);
						// 적중 UI 애니메이션 재생
						crosshairUI->PlayAnimation(crosshairUI->HeadHitAppearAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
						// 데미지 위젯 설정 및 파티클 생성
						SetDamageWidget(DamageAmount * 0.1f, HitResult.Location, true, FLinearColor::Gray);
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(2.f));
					}
					else
					{
						UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
						// 적중 UI 애니메이션 재생
						crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
						// 데미지 위젯 설정 및 파티클 생성
						SetDamageWidget(DamageAmount * 0.05f, HitResult.Location, true, FLinearColor::Gray);
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
				// 적중 파티클 생성
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(2.f));
			}
			else
			{
				// 적중 사운드 재생
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, HitResult.Location);
				// 적중 파티클 생성
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(1.f));
			}
		}
	}
}


void APlayerCharacter::OnContainerHit(const FHitResult& HitResult, ARewardContainer* HitContainer)
{
	// 컨테이너가 파괴되지 않은 경우
	if (!HitContainer->IsBoxDestroyed)
	{
		// 서버에서 OnContainerHitRPCServer 함수 호출
		OnContainerHitRPCServer(HitResult, HitContainer);
	}
}

void APlayerCharacter::OnContainerHitRPCServer_Implementation(const FHitResult& HitResult, ARewardContainer* HitContainer)
{
	// 멀티캐스트 함수 호출
	OnContainerHitRPCMulticast(HitResult, HitContainer);
}

bool APlayerCharacter::OnContainerHitRPCServer_Validate(const FHitResult& HitResult, ARewardContainer* HitContainer)
{
	// 유효성 검사
	return true;
}

void APlayerCharacter::OnContainerHitRPCMulticast_Implementation(const FHitResult& HitResult, ARewardContainer* HitContainer)
{
	// 서버 권한이 있는 경우
	if (HasAuthority())
	{
		// 컨테이너의 현재 HP가 1 이하인 경우
		if (HitContainer->CurBoxHP <= 1)
		{
			// 컨테이너 파괴 함수 호출
			HitContainer->BoxDestroyed();
		}
		else
		{
			// 컨테이너의 HP 감소
			HitContainer->CurBoxHP = FMath::Clamp(HitContainer->CurBoxHP - 1, 0, 5);
		}
	}
	else
	{
		// 로컬 플레이어인 경우
		if (IsLocallyControlled())
		{
			// 크로스헤어 애니메이션 재생
			crosshairUI->PlayAnimation(crosshairUI->HitAppearAnimation);
			// 총알 히트 사운드 재생
			UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound);
		}
		else
		{
			// 총알 히트 사운드를 컨테이너 위치에서 재생
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletHitSound, HitContainer->GetActorLocation());
		}
		// 히트 방향을 회전으로 변환
		const FRotator HitRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		// 히트 위치에서 이펙트 스폰
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, HitResult.Location, HitRot, FVector(1.f));
	}
}

void APlayerCharacter::OnGroundHit(const FHitResult& HitResult)
{
	// 서버에서 OnGroundHitRPCServer 함수 호출
	OnGroundHitRPCServer(HitResult);
}

void APlayerCharacter::OnGroundHitRPCServer_Implementation(const FHitResult& HitResult)
{
	// 멀티캐스트 함수 호출
	OnGroundHitRPCMulticast(HitResult);
}

bool APlayerCharacter::OnGroundHitRPCServer_Validate(const FHitResult& HitResult)
{
	// 유효성 검사
	return true;
}

void APlayerCharacter::OnGroundHitRPCMulticast_Implementation(const FHitResult& HitResult)
{
	// 로컬 플레이어인 경우
	if (IsLocallyControlled())
	{
		// 히트 방향을 회전으로 변환
		const FRotator DecalRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		// 히트 위치를 네트워크 양식으로 저장
		const FVector_NetQuantize DecalLoc = HitResult.Location;
		// 히트 위치와 방향을 변환으로 생성
		const FTransform DecalTrans = UKismetMathLibrary::MakeTransform(DecalLoc, DecalRot);
		// 데칼 스폰
		GetWorld()->SpawnActor<AActor>(ShotDecalFactory, DecalTrans);
		// 히트 위치에 파티클 이펙트 스폰
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, DecalLoc, DecalRot + FRotator(-90, 0, 0), FVector(0.5f));
	}
	else
	{
		// 로컬 플레이어가 아닌 경우
		const FRotator DecalRot = UKismetMathLibrary::Conv_VectorToRotator(HitResult.ImpactNormal);
		const FVector_NetQuantize DecalLoc = HitResult.Location;
		const FTransform DecalTrans = UKismetMathLibrary::MakeTransform(DecalLoc, DecalRot);
		GetWorld()->SpawnActor<AActor>(ShotDecalFactory, DecalTrans);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, DecalLoc, DecalRot + FRotator(-90, 0, 0), FVector(0.5f));
		// 히트 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GroundHitSound, DecalLoc);
	}
}

int32 APlayerCharacter::GenerateRandomDamage(const float InDamage) const
{
	// 주어진 데미지를 기반으로 무작위 데미지 생성
	const double DoubleRandDamage = FMath::FRandRange(InDamage * 0.8, InDamage * 1.2);
	// 반올림하여 정수로 변환
	const int32 RoundedRandDamage = FMath::RoundHalfToEven(DoubleRandDamage);
	return RoundedRandDamage;
}

void APlayerCharacter::SetDamageWidget(const int Damage, const FVector& SpawnLoc, const bool bIsShieldIconEnable, const FLinearColor DamageTextColor)
{
	// 데미지 위젯 액터 생성
	if (const ADamageWidgetActor* DamWidget = GetWorld()->SpawnActor<ADamageWidgetActor>(damageWidgetFactory, SpawnLoc + FVector(0, 0, 50), FRotator::ZeroRotator))
	{
		// 위젯 컴포넌트에서 사용자 위젯 객체 가져오기
		if (UUserWidget* Widui = DamWidget->DamageWidgetComponent->GetUserWidgetObject())
		{
			// 사용자 위젯을 UDamageWidget으로 캐스팅
			damageWidgetUI = Cast<UDamageWidget>(Widui);
			if (damageWidgetUI)
			{
				// 데미지 텍스트 색상 설정
				damageWidgetUI->damageText->SetColorAndOpacity(DamageTextColor);
				// 데미지 설정
				damageWidgetUI->damage = Damage;
				// 실드 아이콘 표시 설정
				if (bIsShieldIconEnable)
				{
					damageWidgetUI->ShieldImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}
				else
				{
					damageWidgetUI->ShieldImage->SetVisibility(ESlateVisibility::Hidden);
				}
				// 무기 타입에 따라 탄약 이미지 설정
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
				// 데미지 애니메이션 재생
				damageWidgetUI->PlayAnimation(damageWidgetUI->DamageFloat);
			}
		}
	}
}


void APlayerCharacter::InteractionProcess()
{
	if (IsLocallyControlled())
	{
		const FVector StartLoc = FollowCamera->GetComponentLocation();
		const FVector EndLoc = StartLoc + FollowCamera->GetForwardVector() * 500.0f;
		// 무기 액터 탐지 라인 트레이스
		if (FHitResult ActorHitResult; GetWorld()->LineTraceSingleByChannel(ActorHitResult, StartLoc, EndLoc, ECC_Visibility))
		{
			// 무기 액터 캐스팅
			rifleActor = Cast<ARifleActor>(ActorHitResult.GetActor());
			sniperActor = Cast<ASniperActor>(ActorHitResult.GetActor());
			pistolActor = Cast<APistolActor>(ActorHitResult.GetActor());
			m249Actor = Cast<AM249Actor>(ActorHitResult.GetActor());
			PickableItemActor = Cast<APickableActor>(ActorHitResult.GetActor());
			MissionChecker = Cast<AMissionChecker>(ActorHitResult.GetActor());
			StageBoard = Cast<AStageBoard>(ActorHitResult.GetActor());
			Stash = Cast<AStash>(ActorHitResult.GetActor());
			Trader = Cast<ATrader>(ActorHitResult.GetActor());
			QuitGameActor = Cast<AQuitGameActor>(ActorHitResult.GetActor());
			DeadPlayerContainer = Cast<ADeadPlayerContainer>(ActorHitResult.GetActor());

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
						PlayerInputComponentRef->ChangeWeaponToRifle(rifleActor);
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
						PlayerInputComponentRef->ChangeWeaponToSniper(sniperActor);
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
						PlayerInputComponentRef->ChangeWeaponToPistol(pistolActor);
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
						PlayerInputComponentRef->ChangeWeaponToM249(m249Actor);
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
					PlayerInputComponentRef->PickableItemActorInteraction(PickableItemActor);
				}
			}
			else if (MissionChecker)
			{
				// 키다운 시간 동안 Radial Slider 게이지 상승
				infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
				if (infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
				{
					infoWidgetUI->weaponHoldPercent = 0;
					MoveToAnotherLevel();

					FTimerHandle TimerHandle;
					GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void
					{
						MoveToHideout(false);
					}), 8.f, false);
				}
			}

			else if (StageBoard)
			{
				// 키다운 시간 동안 Radial Slider 게이지 상승
				infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
				if (levelSelectionUI && !levelSelectionUI->IsInViewport() && infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
				{
					infoWidgetUI->weaponHoldPercent = 0;
					UGameplayStatics::PlaySound2D(GetWorld(), levelSelectionSound);
					infoWidgetUI->RemoveFromParent();
					UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, levelSelectionUI);
					PC->SetShowMouseCursor(true);
					levelSelectionUI->AddToViewport();
				}
			}
			else if (Trader)
			{
				// 키다운 시간 동안 Radial Slider 게이지 상승
				infoWidgetUI->weaponHoldPercent = FMath::Clamp(infoWidgetUI->weaponHoldPercent + 0.02, 0, 1);
				if (TradeWidgetUI && !TradeWidgetUI->IsInViewport() && infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
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
				if (quitWidgetUI && !quitWidgetUI->IsInViewport() && infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
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
				if (infoWidgetUI && infoWidgetUI->weaponHoldPercent >= 1)
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

void APlayerCharacter::MoveToIsolatedShip()
{
	MoveToAnotherLevel();
	bHideout = false;

	FTimerHandle TimerHandle;
	// 타이머를 설정하여 8초 후에 MoveToIsolatedShipClient 함수를 호출
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		MoveToIsolatedShipClient();
	}), 8.f, false);
}

void APlayerCharacter::MoveToIsolatedShipClient()
{
	if (IsLocallyControlled())
	{
		// 플레이어 컨트롤러 입력 허용 설정
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);
		const FName IntersectionLevelName = FName("Deserted_Road");
		const FName SpacecraftLevelName = FName("Map_BigStarStation");
		const FName HideoutLevelName = FName("Safe_House");
		const FName OnSpacecraftStreamingLevelLoadFinished = FName("OnSpacecraftStreamingLevelLoadFinished");
		FLatentActionInfo LoadLatentInfo;
		LoadLatentInfo.CallbackTarget = this;
		LoadLatentInfo.Linkage = 0;
		LoadLatentInfo.ExecutionFunction = OnSpacecraftStreamingLevelLoadFinished;

		// 우주선 레벨을 스트리밍 방식으로 로드
		UGameplayStatics::LoadStreamLevel(this, SpacecraftLevelName, true, true, LoadLatentInfo);
		// 여러 스트리밍 레벨 언로드
		UnloadMultipleStreamingLevels(IntersectionLevelName, HideoutLevelName);
	}
}

void APlayerCharacter::MoveToHideout(const bool IsPlayerDeath)
{
	bHideout = true;

	if (IsPlayerDeath)
	{
		IsPlayerDead = false;
		StopAnimMontage();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ResetPlayerInventoryDataServer();
		ResetTabWidget();
	}
	else
	{
		UpdateTabWidgetInventory();
	}

	if (IsLocallyControlled())
	{
		PC->SetIgnoreLookInput(false);
		PC->SetIgnoreMoveInput(false);

		if (SpawnedSpacecraftSound) SpawnedSpacecraftSound->Deactivate();
		if (SpawnedIntersectionSound) SpawnedIntersectionSound->Deactivate();

		const FName HideoutLevelName = FName("Safe_House");
		const FName IntersectionLevelName = FName("Deserted_Road");
		const FName SpacecraftLevelName = FName("Map_BigStarStation");
		const FName OnHideoutLevelLoadFinishedFunc = FName("OnHideoutStreamingLevelLoadFinished");
		FLatentActionInfo LoadLatentInfo;
		LoadLatentInfo.CallbackTarget = this;
		LoadLatentInfo.Linkage = 0;
		LoadLatentInfo.ExecutionFunction = OnHideoutLevelLoadFinishedFunc;

		// 은신처 레벨을 스트리밍 방식으로 로드
		UGameplayStatics::LoadStreamLevel(this, HideoutLevelName, true, true, LoadLatentInfo);
		// 여러 스트리밍 레벨 언로드
		UnloadMultipleStreamingLevels(IntersectionLevelName, SpacecraftLevelName);
	}
}

void APlayerCharacter::ResetPlayerInventoryDataServer_Implementation()
{
	if (const auto ResetPlayerState = Cast<AEclipsePlayerState>(GetPlayerState()))
	{
		ResetPlayerState->ApplyGearInventoryEquipState(this);
		ResetPlayerState->ResetPlayerInventoryData();
	}
	Stat->SetHp(Stat->GetMaxHp());
}

void APlayerCharacter::MoveToBlockedIntersection()
{
	MoveToAnotherLevel();
	bHideout = false;

	FTimerHandle EndHandle;
	// 타이머를 설정하여 8초 후에 MoveToBlockedIntersectionClient 함수를 호출
	GetWorldTimerManager().SetTimer(EndHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		MoveToBlockedIntersectionClient();
	}), 8.f, false);
}

void APlayerCharacter::MoveToBlockedIntersectionClient()
{
	if (IsLocallyControlled())
	{
		// 플레이어 컨트롤러 입력 허용 설정
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);
		const FName IntersectionLevelName = FName("Deserted_Road");
		const FName HideoutLevelName = FName("Safe_House");
		const FName SpacecraftLevelName = FName("Map_BigStarStation");
		const FName OnIntersectionLevelLoadFinishedFunc = FName("OnIntersectionStreamingLevelLoadFinished");
		FLatentActionInfo LoadLatentInfo;
		LoadLatentInfo.CallbackTarget = this;
		LoadLatentInfo.Linkage = 0;
		LoadLatentInfo.ExecutionFunction = OnIntersectionLevelLoadFinishedFunc;

		// 교차로 레벨을 스트리밍 방식으로 로드
		UGameplayStatics::LoadStreamLevel(this, IntersectionLevelName, true, true, LoadLatentInfo);
		// 여러 스트리밍 레벨 언로드
		UnloadMultipleStreamingLevels(HideoutLevelName, SpacecraftLevelName);
	}
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

void APlayerCharacter::SetTiltingLeftValue(const float Value)
{
	if (TiltReleaseLeft)
	{
		CameraCurrentRotation = FirstPersonCamera->GetRelativeRotation();
		CameraDesiredRotation = FRotator::ZeroRotator;
	}
	else
	{
		CameraCurrentRotation = FirstPersonCamera->GetRelativeRotation();
		CameraDesiredRotation = FRotator(0, 0, -15);
	}

	// RLerp와 TimeLine Value 값을 통한 자연스러운 기울이기
	const FRotator RLerp = UKismetMathLibrary::RLerp(CameraCurrentRotation, CameraDesiredRotation, Value, true);

	// 해당 트랜스폼 할당	
	FirstPersonCamera->SetRelativeRotation(RLerp);
}

void APlayerCharacter::SetTiltingRightValue(const float Value)
{
	if (TiltReleaseRight)
	{
		CameraCurrentRotation = FirstPersonCamera->GetRelativeRotation();
		CameraDesiredRotation = FRotator::ZeroRotator;
	}
	else
	{
		CameraCurrentRotation = FirstPersonCamera->GetRelativeRotation();
		CameraDesiredRotation = FRotator(0, 0, 15);
	}

	// RLerp와 TimeLine Value 값을 통한 자연스러운 기울이기
	const FRotator RLerp = UKismetMathLibrary::RLerp(CameraCurrentRotation, CameraDesiredRotation, Value, true);
	// 해당 트랜스폼 할당
	FirstPersonCamera->SetRelativeRotation(RLerp);
}

void APlayerCharacter::Damaged(const int Damage, AActor* DamageCauser)
{
	Stat->ApplyDamage(Damage, DamageCauser);
	DamagedRPCServer(Damage, DamageCauser);
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
	if (IsLocallyControlled())
	{
		crosshairUI->PlayAnimation(crosshairUI->DamagedAnimation, 0, 1, EUMGSequencePlayMode::Forward, 1, true);
		UGameplayStatics::PlaySound2D(GetWorld(), DamagedSound);
		PC->PlayerCameraManager->StartCameraShake(PlayerDamagedShake);
	}
	if (!HasAuthority())
	{
		UpdateTabWidget();
		PlayAnimMontage(FullBodyMontage, 1, FName("Damaged"));
		FTimerHandle OverlayMatHandle;
		GetMesh()->SetOverlayMaterial(overlayMatRed);
		GetWorldTimerManager().ClearTimer(OverlayMatHandle);
		GetWorldTimerManager().SetTimer(OverlayMatHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			GetMesh()->SetOverlayMaterial(nullptr);
		}), 0.3f, false);
	}
}

void APlayerCharacter::OnRep_WeaponArrayChanged()
{
	WeaponChangeDele.Broadcast();
	if (!HasAuthority())
	{
		for (int32 i = 0; i < weaponArray.Num(); ++i)
		{
			if (weaponArray[i] == true)
			{
				ModifyFlashlightAttachment(i);
				return;
			}
		}
	}
}

void APlayerCharacter::AddAmmunitionByInputString(const FString& InventoryStructName)
{
	if (InventoryStructName.Contains(TEXT("Rifle")))
	{
		maxRifleAmmo += AdditionalRifleMag;
	}
	else if (InventoryStructName.Contains(TEXT("Sniper")))
	{
		maxSniperAmmo += AdditionalSniperMag;
	}
	else if (InventoryStructName.Contains(TEXT("Pistol")))
	{
		maxPistolAmmo += AdditionalPistolMag;
	}
	else if (InventoryStructName.Contains(TEXT("M249")))
	{
		maxM249Ammo += AdditionalM249Mag;
	}
}

void APlayerCharacter::OnSpacecraftStreamingLevelLoadFinished()
{
	if (IsLocallyControlled())
	{
		IsPlayerDeadImmediately = false;
		if (!crosshairUI->IsInViewport()) crosshairUI->AddToViewport();
		if (!informationUI->IsInViewport()) informationUI->AddToViewport();
		informationUI->EnterSpacecraft();
		UGameplayStatics::PlaySound2D(GetWorld(), PlayerSpawnSound, 0.6, 1, 0.25);
		SpawnedSpacecraftSound = UGameplayStatics::SpawnSound2D(GetWorld(), SpacecraftAmbientSound);
		GetCharacterMovement()->MaxWalkSpeed = CharacterDefaultWalkSpeed;

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			if (APlayerCameraManager* const CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)))
			{
				CameraManager->StopCameraFade();
				CameraManager->StartCameraFade(1.0, 0, 10.0, FColor::Black, false, false);
			}
		}), 0.5f, false);
	}

	OnRep_WeaponArrayChanged();
	bEnding = false;

	OnSpacecraftStreamingLevelLoadFinishedServer();
}

void APlayerCharacter::OnSpacecraftStreamingLevelLoadFinishedServer_Implementation()
{
	bSpacecraft = true;
	IsPlayerDeadImmediately = false;
	InitializeAccumulatedDamage();
	ChoosePlayerStartByTagName(FName("Spacecraft"), 50);
}

void APlayerCharacter::OnIntersectionStreamingLevelLoadFinished()
{
	if (IsLocallyControlled())
	{
		IsPlayerDeadImmediately = false;
		if (!crosshairUI->IsInViewport()) crosshairUI->AddToViewport();
		if (!informationUI->IsInViewport()) informationUI->AddToViewport();
		informationUI->EnterIntersection();
		UGameplayStatics::PlaySound2D(GetWorld(), PlayerSpawnSound, 0.6, 1, 0.25);
		SpawnedIntersectionSound = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), IntersectionAmbientSound, FVector(234, -800, 0));
		GetCharacterMovement()->MaxWalkSpeed = CharacterDefaultWalkSpeed;

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			if (APlayerCameraManager* const CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)))
			{
				CameraManager->StopCameraFade();
				CameraManager->StartCameraFade(1.0, 0, 10.0, FColor::Black, false, false);
			}
		}), 0.5f, false);
	}

	OnRep_WeaponArrayChanged();
	bEnding = false;

	OnIntersectionStreamingLevelLoadFinishedServer();
}


void APlayerCharacter::OnIntersectionStreamingLevelLoadFinishedServer_Implementation()
{
	bSpacecraft = false;
	IsPlayerDeadImmediately = false;
	InitializeAccumulatedDamage();
	ChoosePlayerStartByTagName(FName("Intersection"), 3000);
}

void APlayerCharacter::OnHideoutStreamingLevelLoadFinished()
{
	if (IsLocallyControlled())
	{
		IsPlayerDeadImmediately = false;
		if (!crosshairUI->IsInViewport()) crosshairUI->AddToViewport();
		if (!informationUI->IsInViewport()) informationUI->AddToViewport();
		informationUI->EnterHideout();

		UGameplayStatics::PlaySound2D(GetWorld(), PlayerSpawnSound, 0.6, 1, 0.25);

		GetCharacterMovement()->MaxWalkSpeed = CharacterDefaultWalkSpeed;

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			if (APlayerCameraManager* const CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)))
			{
				CameraManager->StopCameraFade();
				CameraManager->StartCameraFade(1.0, 0, 10.0, FColor::Black, false, false);
			}
		}), 0.5f, false);

		OnRep_WeaponArrayChanged();
		bEnding = false;
	}

	OnHideoutStreamingLevelLoadFinishedServer();
}

void APlayerCharacter::OnHideoutStreamingLevelLoadFinishedServer_Implementation()
{
	bSpacecraft = false;
	IsPlayerDeadImmediately = false;
	Stat->SetHp(Stat->GetMaxHp());
	InitializeAccumulatedDamage();
	ChoosePlayerStartByTagName(FName("Hideout"), 50);
	OnHideoutStreamingLevelLoadFinishedMulticast();
}

void APlayerCharacter::OnHideoutStreamingLevelLoadFinishedMulticast_Implementation()
{
	if (!HasAuthority() && !IsLocallyControlled())
	{
		if (!GetMesh()->IsVisible())
		{
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void
			{
				GetMesh()->SetVisibility(true);
			}), 2.f, false);
		}
	}
}

void APlayerCharacter::ModifyFlashlightAttachment(const int32 WeaponNum) const
{
	const FName SocketName = TEXT("Flashlight");
	if (WeaponNum == 0)
	{
		FlashLight->SetWorldTransform(RifleComp->GetSocketTransform(SocketName));
		if (!RifleComp->IsRegistered()) RifleComp->RegisterComponent();
		FlashLight->AttachToComponent(RifleComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
	else if (WeaponNum == 1)
	{
		FlashLight->SetWorldTransform(SniperComp->GetSocketTransform(SocketName));
		if (!SniperComp->IsRegistered()) SniperComp->RegisterComponent();
		FlashLight->AttachToComponent(SniperComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
	else if (WeaponNum == 2)
	{
		FlashLight->SetWorldTransform(PistolComp->GetSocketTransform(SocketName));
		if (!PistolComp->IsRegistered()) PistolComp->RegisterComponent();
		FlashLight->AttachToComponent(PistolComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
	else if (WeaponNum == 3)
	{
		FlashLight->SetWorldTransform(M249Comp->GetSocketTransform(SocketName));
		if (!M249Comp->IsRegistered()) M249Comp->RegisterComponent();
		FlashLight->AttachToComponent(M249Comp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
}

void APlayerCharacter::InitializeAccumulatedDamage() const
{
	Stat->AccumulatedDamageToBoss = 0;
	Stat->AccumulatedDamageToEnemy = 0;
	Stat->AccumulatedDamageToPlayer = 0;
}

void APlayerCharacter::ChoosePlayerStartByTagName(const FName& PlayerStartTagName, const int32 DetectionSphereRadius)
{
	TArray<class AActor*> OutActors;
	TArray<class APlayerStart*> TargetPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerStartFactory, OutActors);
	for (const auto PlayerStarts : OutActors)
	{
		if (const auto PlayerStart = Cast<APlayerStart>(PlayerStarts))
		{
			// 대상 PlayerStartTag가 매개변수의 FName과 일치한다면
			if (PlayerStart && PlayerStart->PlayerStartTag == PlayerStartTagName)
			{
				const FVector Center = PlayerStart->GetActorLocation();
				FCollisionQueryParams Params;
				bool AlreadyLocated = false;
				// PlayerStart주변 캐릭터 여부 탐지를 위한 오버랩 멀티
				if (TArray<FOverlapResult> HitObj; GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(DetectionSphereRadius), Params))
				{
					for (int i = 0; i < HitObj.Num(); ++i)
					{
						// 일정 반경 내에 캐릭터가 존재한다면
						if (Cast<APlayerCharacter>(HitObj[i].GetActor()))
						{
							AlreadyLocated = true;
							// 반복문을 빠져나온다.
							break;
						}
					}
				}
				// 일정 반경 내에 캐릭터가 존재하는 경우
				if (AlreadyLocated)
				{
					// TargetPlayerStart 배열에 추가하지 않고 넘어간다.
					continue;
				}
				// 일정 반경 내에 캐릭터가 존재하지 않는 경우 TargetPlayerStart 배열에 추가한다.
				TargetPlayerStarts.Add(PlayerStart);
			}
		}
	}
	if (const auto PlayerStartRandIndex = FMath::RandRange(0, TargetPlayerStarts.Num() - 1); TargetPlayerStarts.IsValidIndex(PlayerStartRandIndex))
	{
		// 조건에 부합하는 PlayerStart 중 랜덤한 인덱스의 PlayerStart 트랜스폼
		const FTransform TargetPlayerStartTrans = TargetPlayerStarts[PlayerStartRandIndex]->GetActorTransform();
		// 선별한 PlayerStart 지점으로 플레이어 이동
		SetActorTransform(TargetPlayerStartTrans, false);
		// 캐릭터의 Control Rotation 설정 클라이언트 RPC 함수
		SetPlayerControlRotation(TargetPlayerStartTrans.Rotator());
	}
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
	if (!HasAuthority())
	{
		if (IsEquipArmor)
		{
			ArmorSlot->SetVisibility(true);
			if (IsLocallyControlled())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), gearEquipSound);
			}
		}
		else
		{
			ArmorSlot->SetVisibility(false);
			if (IsLocallyControlled())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), gearUnequipSound);
			}
		}
	}
}

void APlayerCharacter::OnRep_IsEquipHelmet() const
{
	if (!HasAuthority())
	{
		if (IsEquipHelmet)
		{
			HelmetSlot->SetVisibility(true);
			if (IsLocallyControlled())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), gearEquipSound);
			}
		}
		else
		{
			HelmetSlot->SetVisibility(false);
			if (IsLocallyControlled())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), gearUnequipSound);
			}
		}
	}
}

void APlayerCharacter::OnRep_IsEquipGoggle() const
{
	if (!HasAuthority())
	{
		if (IsEquipGoggle)
		{
			GoggleSlot->SetVisibility(true);
			if (IsLocallyControlled())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), gearEquipSound);
			}
		}
		else
		{
			GoggleSlot->SetVisibility(false);
			if (IsLocallyControlled())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), gearUnequipSound);
			}
		}
	}
}

void APlayerCharacter::OnRep_IsEquipMask() const
{
	if (!HasAuthority())
	{
		if (IsEquipMask)
		{
			MaskSlot->SetVisibility(true);
			if (IsLocallyControlled())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), gearEquipSound);
			}
		}
		else
		{
			MaskSlot->SetVisibility(false);
			if (IsLocallyControlled())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), gearUnequipSound);
			}
		}
	}
}

void APlayerCharacter::OnRep_IsEquipHeadset() const
{
	if (!HasAuthority())
	{
		if (IsEquipHeadset)
		{
			HeadSetSlot->SetVisibility(true);
			if (IsLocallyControlled())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), gearEquipSound);
			}
		}
		else
		{
			HeadSetSlot->SetVisibility(false);
			if (IsLocallyControlled())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), gearUnequipSound);
			}
		}
	}
}

void APlayerCharacter::OnRep_MaxRifleAmmo()
{
	UpdateAmmunition();
}

void APlayerCharacter::OnRep_MaxSniperAmmo()
{
	UpdateAmmunition();
}

void APlayerCharacter::OnRep_MaxPistolAmmo()
{
	UpdateAmmunition();
}

void APlayerCharacter::OnRep_MaxM249Ammo()
{
	UpdateAmmunition();
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
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PlayerSpawnEmitter, GetActorLocation());
	UGameplayStatics::PlaySound2D(GetWorld(), ExtractionSound);
	APlayerCameraManager* PlayerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	PlayerCam->StartCameraFade(0, 1, 2.0, FLinearColor::Black, false, true);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		MoveToHideout(false);
	}), 2.f, false);
}

void APlayerCharacter::SetFirstPersonModeRifle(const bool IsFirstPerson)
{
	if (IsFirstPerson)
	{
		bUseControllerRotationPitch = true;
		bUseControllerRotationRoll = true;
		crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
		GetMesh()->SetVisibility(false);
		RifleComp->SetVisibility(false);
		FirstPersonRifleComp->SetVisibility(true);
		FirstPersonCharacterMesh->SetVisibility(true);
		FollowCamera->SetActive(false);
		FirstPersonCamera->SetActive(true);

		FlashLight->SetWorldTransform(FirstPersonRifleComp->GetSocketTransform(FName("FirstFlashlight")));
		FlashLight->AttachToComponent(FirstPersonRifleComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("FirstFlashlight"));

		if (const auto FirstAnimInstance = Cast<UFirstPersonPlayerAnim>(FirstPersonCharacterMesh->GetAnimInstance()))
		{
			FirstAnimInstance->bPistol = false;
			FirstAnimInstance->Montage_Play(FirstPersonRifeZoomMontage, 1);
		}
	}
	else
	{
		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));
		crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Visible);
		GetMesh()->SetVisibility(true);
		if (weaponArray[0] == true)
		{
			RifleComp->SetVisibility(true);
		}
		FirstPersonRifleComp->SetVisibility(false);
		FirstPersonCharacterMesh->SetVisibility(false);
		FollowCamera->SetActive(true);
		FirstPersonCamera->SetActive(false);

		FlashLight->SetWorldTransform(RifleComp->GetSocketTransform(FName("Flashlight")));
		FlashLight->AttachToComponent(RifleComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Flashlight"));
	}
}

void APlayerCharacter::SetFirstPersonModePistol(const bool IsFirstPerson)
{
	if (IsFirstPerson)
	{
		bUseControllerRotationPitch = true;
		bUseControllerRotationRoll = true;
		crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
		GetMesh()->SetVisibility(false);
		PistolComp->SetVisibility(false);
		FirstPersonPistolComp->SetVisibility(true);
		FirstPersonCharacterMesh->SetVisibility(true);
		FollowCamera->SetActive(false);
		FirstPersonCamera->SetActive(true);

		FlashLight->SetWorldTransform(FirstPersonPistolComp->GetSocketTransform(FName("Flashlight")));
		FlashLight->AttachToComponent(FirstPersonPistolComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Flashlight"));

		if (const auto FirstAnimInstance = Cast<UFirstPersonPlayerAnim>(FirstPersonCharacterMesh->GetAnimInstance()))
		{
			FirstAnimInstance->bPistol = true;
			FirstAnimInstance->Montage_Play(FirstPersonPistolZoomMontage, 1);
		}
	}
	else
	{
		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;
		SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));
		crosshairUI->CrosshairImage->SetVisibility(ESlateVisibility::Visible);
		GetMesh()->SetVisibility(true);
		if (weaponArray[2] == true)
		{
			PistolComp->SetVisibility(true);
		}
		FirstPersonPistolComp->SetVisibility(false);
		FirstPersonCharacterMesh->SetVisibility(false);
		FollowCamera->SetActive(true);
		FirstPersonCamera->SetActive(false);

		FlashLight->SetWorldTransform(PistolComp->GetSocketTransform(FName("Flashlight")));
		FlashLight->AttachToComponent(PistolComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Flashlight"));
	}
}

void APlayerCharacter::EquipArmorInventorySlot(const bool IsEquipping, const float EquipGearStat)
{
	// 장착 중인지 확인
	if (IsEquipping)
	{
		// 서버 권한이 있는지 확인
		if (HasAuthority())
		{
			// 최대 체력 증가
			Stat->AddMaxHp(EquipGearStat);
		}
		// 갑옷 장착 상태를 true로 설정
		IsEquipArmor = true;
		OnRep_IsEquipArmor();
	}
	else
	{
		// 서버 권한이 있는지 확인
		if (HasAuthority())
		{
			// 최대 체력 감소
			Stat->SubtractMaxHp(EquipGearStat);
		}
		// 갑옷 장착 상태를 false로 설정
		IsEquipArmor = false;
		OnRep_IsEquipArmor();
	}
}

void APlayerCharacter::EquipHelmetInventorySlot(const bool IsEquipping, const float EquipGearStat)
{
	// 헬멧 장착 중인지 확인
	if (IsEquipping)
	{
		// 헬멧 장착 상태를 true로 설정
		IsEquipHelmet = true;
		OnRep_IsEquipHelmet();
	}
	else
	{
		// 헬멧 장착 상태를 false로 설정
		IsEquipHelmet = false;
		OnRep_IsEquipHelmet();
	}
}

void APlayerCharacter::EquipGoggleInventorySlot(const bool IsEquipping, const float EquipGearStat)
{
	// 고글 장착 중인지 확인
	if (IsEquipping)
	{
		// 서버 권한이 있는지 확인
		if (HasAuthority())
		{
			// 반동 통계 배율 설정
			Stat->RecoilStatMultiplier = EquipGearStat;
		}
		// 고글 장착 상태를 true로 설정
		IsEquipGoggle = true;
		OnRep_IsEquipGoggle();
	}
	else
	{
		// 서버 권한이 있는지 확인
		if (HasAuthority())
		{
			// 반동 통계 배율 설정
			Stat->RecoilStatMultiplier = EquipGearStat;
		}
		// 고글 장착 상태를 false로 설정
		IsEquipGoggle = false;
		OnRep_IsEquipGoggle();
	}
}

void APlayerCharacter::EquipHeadsetInventorySlot(const bool IsEquipping, const float EquipGearStat)
{
	// 헤드셋 장착 중인지 확인
	if (IsEquipping)
	{
		// 서버 권한이 있는지 확인
		if (HasAuthority())
		{
			// 데미지 통계 배율 설정
			Stat->DamageStatMultiplier = EquipGearStat;
		}
		// 헤드셋 장착 상태를 true로 설정
		IsEquipHeadset = true;
		OnRep_IsEquipHeadset();
	}
	else
	{
		// 서버 권한이 있는지 확인
		if (HasAuthority())
		{
			// 데미지 통계 배율 설정
			Stat->DamageStatMultiplier = EquipGearStat;
		}
		// 헤드셋 장착 상태를 false로 설정
		IsEquipHeadset = false;
		OnRep_IsEquipHeadset();
	}
}

void APlayerCharacter::EquipMaskInventorySlot(const bool IsEquipping, const float EquipGearStat)
{
	// 마스크 장착 중인지 확인
	if (IsEquipping)
	{
		// 서버 권한이 있는지 확인
		if (HasAuthority())
		{
			// 발사 간격 통계 배율 설정
			Stat->FireIntervalStatMultiplier = EquipGearStat;
		}
		// 마스크 장착 상태를 true로 설정
		IsEquipMask = true;
		OnRep_IsEquipMask();
	}
	else
	{
		// 서버 권한이 있는지 확인
		if (HasAuthority())
		{
			// 발사 간격 통계 배율 설정
			Stat->FireIntervalStatMultiplier = EquipGearStat;
		}
		// 마스크 장착 상태를 false로 설정
		IsEquipMask = false;
		OnRep_IsEquipMask();
	}
}

void APlayerCharacter::DeadPlayerContainerSettings(ADeadPlayerContainer* DeadPlayerContainers) const
{
	// DeadPlayerContainers가 유효한지 확인
	if (DeadPlayerContainers)
	{
		// 플레이어 상태를 AEclipsePlayerState로 캐스팅
		if (const AEclipsePlayerState* EcPlayerState = Cast<AEclipsePlayerState>(GetPlayerState()))
		{
			// 사망한 플레이어의 인벤토리 데이터 설정
			DeadPlayerContainers->DeadPlayerInventoryStructArray = EcPlayerState->GetInventoryStructs();
			DeadPlayerContainers->DeadPlayerInventoryStackArray = EcPlayerState->GetInventoryStacks();
			DeadPlayerContainers->DeadPlayerGearSlotArray = EcPlayerState->GetGearSlotStructs();
		}
	}
}

void APlayerCharacter::PlayerDeath()
{
	// 로컬 플레이어인지 확인
	if (IsLocallyControlled())
	{
		// 줌 해제
		PlayerInputComponentRef->ZoomRelease(false);
		// 1인칭 소총 모드 해제
		SetFirstPersonModeRifle(false);
		// 1인칭 권총 모드 해제
		SetFirstPersonModePistol(false);
		// 입력 무시 설정
		PC->SetIgnoreLookInput(true);
		PC->SetIgnoreMoveInput(true);
		// 모든 위젯 제거
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
		// 플레이어 카메라 관리자 가져오기
		APlayerCameraManager* PlayerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		// 카메라 페이드 연출
		PlayerCam->StartCameraFade(0, 1, 7.0, FLinearColor::Black, false, true);
		// 시야각 설정
		FollowCamera->SetFieldOfView(90.f);
	}
	// 타이머 핸들러 설정
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		// 은신처로 이동
		MoveToHideout(true);
	}), 10.f, false);
}

void APlayerCharacter::PlayerDeathRPCServer_Implementation()
{
	// 멀티캐스트 호출
	PlayerDeathRPCMulticast();
}

bool APlayerCharacter::PlayerDeathRPCServer_Validate()
{
	// 유효성 검사
	return true;
}

void APlayerCharacter::PlayerDeathRPCMulticast_Implementation()
{
	// 플레이어 즉시 사망 상태 설정
	IsPlayerDeadImmediately = true;
	// 서버 권한이 없는 경우
	if (!HasAuthority())
	{
		// 몽타주 재생 중단
		StopAnimMontage();
		// 사망 몽타주 재생
		PlayAnimMontage(FullBodyMontage, 1, FName("Death"));
	}
	// 타이머 핸들러 설정
	FTimerHandle PlayerDeadHandle;
	GetWorld()->GetTimerManager().SetTimer(PlayerDeadHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		// 서버 권한이 있는 경우
		if (HasAuthority())
		{
			// 플레이어 사망 상태 설정
			IsPlayerDead = true;
			// 캡슐 컴포넌트 충돌 비활성화
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// 메시 충돌 비활성화
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// 사망한 플레이어 컨테이너 생성
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ADeadPlayerContainer* DeadPlayerBodyActor = GetWorld()->SpawnActor<ADeadPlayerContainer>(DeadPlayerContainerFactory, GetMesh()->GetComponentTransform(), Params);
			// 사망한 플레이어 컨테이너 설정
			DeadPlayerContainerSettings(DeadPlayerBodyActor);
		}
		else
		{
			// 로컬 제어가 아닌 경우 메시 가시성 비활성화
			if (!IsLocallyControlled())
			{
				GetMesh()->SetVisibility(false);
			}
		}

		// 로컬 제어인 경우 사망 소리 재생
		if (IsLocallyControlled())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), DeathSound);
		}
	}), 2.f, false);
}

void APlayerCharacter::PurchaseAmmo(const int32 AmmoIndex)
{
	// 서버에서 PurchaseAmmo 함수 호출
	PurchaseAmmoServer(AmmoIndex);
}

void APlayerCharacter::PurchaseAmmoServer_Implementation(const int32 AmmoIndex)
{
	// 서버 권한이 있는지 확인
	if (HasAuthority())
	{
		// 탄약 인덱스에 따라 탄약 수량 증가
		if (AmmoIndex == 0)
		{
			maxRifleAmmo += 40;
		}
		else if (AmmoIndex == 1)
		{
			maxSniperAmmo += 5;
		}
		else if (AmmoIndex == 2)
		{
			maxPistolAmmo += 8;
		}
		else if (AmmoIndex == 3)
		{
			maxM249Ammo += 50;
		}
	}
}

void APlayerCharacter::MoveToAnotherLevel()
{
	// 엔딩 상태 설정
	bEnding = true;
	gi->IsWidgetOn = false;
	IsPlayerDeadImmediately = true;

	// 로컬 제어인 경우
	if (IsLocallyControlled())
	{
		// 입력 무시 설정
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
		// 위젯 제거
		infoWidgetUI->RemoveFromParent();
		informationUI->RemoveFromParent();
		crosshairUI->RemoveFromParent();

		// 플레이어 카메라 관리자 가져오기
		APlayerCameraManager* PlayerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		// 카메라 페이드 연출
		PlayerCam->StartCameraFade(0, 1, 7.0, FLinearColor::Black, false, true);
		// 포털 소리 재생
		UGameplayStatics::PlaySound2D(GetWorld(), PortalSound);
	}

	// 서버에서 MoveToAnotherLevel 함수 호출
	MoveToAnotherLevelServer();
}

void APlayerCharacter::MoveToAnotherLevelServer_Implementation()
{
	// 멀티캐스트 호출
	MoveToAnotherLevelMulticast();
}

void APlayerCharacter::MoveToAnotherLevelMulticast_Implementation()
{
	// 서버 권한이 없는 경우
	if (!HasAuthority())
	{
		// 이펙트 스폰
		const FTransform SpawnTrans = this->GetTransform();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), recallParticle, SpawnTrans);
		// 레벨 종료 몽타주 재생
		PlayAnimMontage(FullBodyMontage, 1, FName("LevelEnd"));
	}
}

void APlayerCharacter::UnloadMultipleStreamingLevels(const FName& FirstLevelName, const FName& SecondLevelName)
{
	// 언로드할 스트리밍 레벨 배열 설정
	TArray<FName> StreamingLevelArray = {FirstLevelName, SecondLevelName};
	int32 StreamingLevelID = 0;
	// 각 레벨에 대해 언로드 실행
	for (const auto LevelName : StreamingLevelArray)
	{
		FLatentActionInfo LatentActionInfo;
		LatentActionInfo.CallbackTarget = this;
		LatentActionInfo.UUID = StreamingLevelID;
		UGameplayStatics::UnloadStreamLevel(GetWorld(), LevelName, LatentActionInfo, false);
		StreamingLevelID++;
	}
}
