// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerAnim.h"
#include "RifleActor.h"
#include "SniperActor.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PistolActor.h"
#include "WeaponInfoWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
	GetCharacterMovement()->JumpZVelocity = 900.f;
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

	weaponDetectionCollision=CreateDefaultSubobject<USphereComponent>(TEXT("weaponDetectionCollision"));
	weaponDetectionCollision->SetupAttachment(RootComponent);
	weaponDetectionCollision->SetGenerateOverlapEvents(true);


}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	bUsingRifle=true;
	bUsingSniper=false;
	bUsingPistol=false;
	weaponArray.Add(bUsingRifle); //0
	weaponArray.Add(bUsingSniper); //1
	weaponArray.Add(bUsingPistol); //2

	animInstance=Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());

	curRifleAmmo=30;
	curSniperAmmo=5;
	curPistolAmmo=8;

	// Timeline Binding
	if (CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &APlayerCharacter::SetZoomValue);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}

	rifleComp->SetVisibility(true);
	sniperComp->SetVisibility(false);
	pistolComp->SetVisibility(false);

	crosshairUI = CreateWidget<UUserWidget>(GetWorld(), crosshairFactory);
	crosshairUI->AddToViewport();

	infoWidgetUI = CreateWidget<UWeaponInfoWidget>(GetWorld(), infoWidgetFactory);
	
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

	}
}


void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

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
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::Zoom()
{
	isZooming=true;
	// is using sniper
	if(weaponArray[1]==true)
	{
		crosshairUI->RemoveFromParent();
		auto cameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
		cameraManager->StartCameraFade(1.0, 0.1, 2.0, FColor::Black, false, true);
		auto animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
		if(animInst)
		{
			if(animInst->bPistol==false)
			{
				PlayAnimMontage(zoomingMontage, 1, FName("Zooming"));
			}
		}
		Timeline.PlayFromStart();
	}
	else
	{
		auto animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
		if(animInst)
		{
			if(animInst->bPistol==false)
			{
				PlayAnimMontage(zoomingMontage, 1, FName("Zooming"));
			}
		}
		Timeline.PlayFromStart();	
	}
}

void APlayerCharacter::ZoomRelease()
{
	isZooming = false;
	if(weaponArray[1]==true)
	{
		StopAnimMontage();
		crosshairUI->AddToViewport();
		Timeline.ReverseFromEnd();
	}
	else
	{
		StopAnimMontage();
		Timeline.ReverseFromEnd();
	}

}

void APlayerCharacter::OnActionLookAroundPressed()
{
	bUseControllerRotationYaw = false;	
}

void APlayerCharacter::OnActionLookAroundReleased()
{
	bUseControllerRotationYaw = true;
}


void APlayerCharacter::WeaponDetectionLineTrace()
{
	FHitResult actorHitResult;
	FVector StartLoc = FollowCamera->GetComponentLocation();
	FVector EndLoc = StartLoc+FollowCamera->GetForwardVector()*500.0f;
	bool bHit = GetWorld()->LineTraceSingleByChannel(actorHitResult, StartLoc, EndLoc, ECC_Visibility);
	if(bHit)
	{
		rifleActor = Cast<ARifleActor>(actorHitResult.GetActor());
		sniperActor=Cast<ASniperActor>(actorHitResult.GetActor());
		pistolActor=Cast<APistolActor>(actorHitResult.GetActor());
		if(rifleActor)
		{
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				isCursorOnRifle=true;
				rifleActor->weaponMesh->SetRenderCustomDepth(true);
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(0);
				infoWidgetUI->weaponHoldPercent=0;
				infoWidgetUI->AddToViewport();
			}
		}
		else if(sniperActor)
		{
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				isCursorOnSniper=true;
				sniperActor->weaponMesh->SetRenderCustomDepth(true);
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(1);
				infoWidgetUI->weaponHoldPercent=0;
				infoWidgetUI->AddToViewport();
			}
		}
		else if(pistolActor)
		{
			if(TickOverlapBoolean==false)
			{
				TickOverlapBoolean=true;
				isCursorOnPistol=true;
				pistolActor->weaponMesh->SetRenderCustomDepth(true);
				infoWidgetUI->WidgetSwitcher_Weapon->SetActiveWidgetIndex(2);
				infoWidgetUI->weaponHoldPercent=0;
				infoWidgetUI->AddToViewport();
			}
		}
		else
		{
			if(TickOverlapBoolean==true)
			{
				TickOverlapBoolean=false;
				infoWidgetUI->RemoveFromParent();
				// 중심점
				FVector Center = this->GetActorLocation();
				// 충돌체크(구충돌)
				// 충돌한 물체를 기억할 배열
				TArray<FOverlapResult> HitObj;;
				FCollisionQueryParams params;
				params.AddIgnoredActor(this);
				bool bEndOverlapHit = GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(500), params);
				if(bEndOverlapHit)
				{
					int32 rifleArrayNum = 0;
					int32 sniperArrayNum = 0;
					int32 pistolArrayNum = 0;
					for (int i = 0; i < HitObj.Num(); ++i)
					{
						rifleActor = Cast<ARifleActor>(HitObj[i].GetActor());
						sniperActor=Cast<ASniperActor>(HitObj[i].GetActor());
						pistolActor=Cast<APistolActor>(HitObj[i].GetActor());
						if(rifleActor)
						{
							rifleActor->weaponMesh->SetRenderCustomDepth(false);
						}
						else if(sniperActor)
						{
							sniperActor->weaponMesh->SetRenderCustomDepth(false);
						}
						else if(pistolActor)
						{
							pistolActor->weaponMesh->SetRenderCustomDepth(false);
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

void APlayerCharacter::Crouching()
{
}

void APlayerCharacter::ChangeWeapon()
{
		FHitResult actorHitResult;
		FVector StartLoc = FollowCamera->GetComponentLocation();
		FVector EndLoc = StartLoc+FollowCamera->GetForwardVector()*500.0f;
		bool bHit = GetWorld()->LineTraceSingleByChannel(actorHitResult, StartLoc, EndLoc, ECC_Visibility);
		if(bHit)
		{
			rifleActor = Cast<ARifleActor>(actorHitResult.GetActor());
			sniperActor=Cast<ASniperActor>(actorHitResult.GetActor());
			pistolActor=Cast<APistolActor>(actorHitResult.GetActor());
			// 라이플로 교체
			if(rifleActor)
			{
				// 라이플을 사용하지 않을 때만 교체
				if(weaponArray[0]==false)
				{
					infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.01, 0, 1);
					if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
					{
						PlayAnimMontage(zoomingMontage, 1 , FName("WeaponEquip"));
						rifleActor->Destroy();
						FVector spawnPosition = GetMesh()->GetSocketLocation(FName("hand_r"));
						FRotator spawnRotation = FRotator::ZeroRotator;
						FActorSpawnParameters param;
						param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
						// 스나이퍼를 사용중일 때
						if(weaponArray[1]==true)
						{
							sniperActor = GetWorld()->SpawnActor<ASniperActor>(sniperFactory, spawnPosition, spawnRotation);
						}
						// 권총을 사용중일 때
						else if(weaponArray[2]==true)
						{
							auto animInst = Cast<UPlayerAnim>(GetMesh()->GetAnimInstance());
							if(animInst)
							{
								animInst->bPistol=false;
							}
							pistolActor = GetWorld()->SpawnActor<APistolActor>(pistolFactory, spawnPosition, spawnRotation);
						}

						// Visibility 설정
						rifleComp->SetVisibility(true);
						sniperComp->SetVisibility(false);
						pistolComp->SetVisibility(false);

						// 무기 배열 설정
						weaponArray[0]=true;
						weaponArray[1]=false;
						weaponArray[2]=false;
					}
				}
			}
			// 스나이퍼로 교체
			else if(sniperActor)
			{
				// 스나이퍼를 사용하지 않을 때만 교체
				if(weaponArray[1]==false)
				{
					infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.01, 0, 1);
					if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
					{
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
				
						rifleComp->SetVisibility(false);
						sniperComp->SetVisibility(true);
						pistolComp->SetVisibility(false);

						weaponArray[0]=false;
						weaponArray[1]=true;
						weaponArray[2]=false;
					}
				}
			}
			// 권총으로 교체
			else if(pistolActor)
			{
				// 권총을 사용하지 않을 때만 교체
				if(weaponArray[2]==false)
				{
					infoWidgetUI->weaponHoldPercent=FMath::Clamp(infoWidgetUI->weaponHoldPercent+0.01, 0, 1);
					if(infoWidgetUI&&infoWidgetUI->weaponHoldPercent>=1)
					{
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
					
						rifleComp->SetVisibility(false);
						sniperComp->SetVisibility(false);
						pistolComp->SetVisibility(true);
					
						weaponArray[0]=false;
						weaponArray[1]=false;
						weaponArray[2]=true;
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
		if(weaponArray[0]==true&&curRifleAmmo<30)
		{
			PlayAnimMontage(zoomingMontage, 1, FName("Reload"));
		}
		else if(weaponArray[1]==true&&curSniperAmmo<5)
		{
			PlayAnimMontage(zoomingMontage, 1, FName("Reload"));
		}
		else if(weaponArray[2]==true&&curPistolAmmo<8)
		{
			PlayAnimMontage(zoomingMontage, 1, FName("Reload"));
		}
	}
}

void APlayerCharacter::SetZoomValue(float Value)
{
	auto lerp=UKismetMathLibrary::Lerp(200,120,Value);
	CameraBoom->TargetArmLength=lerp;
}


void APlayerCharacter::Fire()
{
	if(!CanShoot)
	{
		return;
	}
	// 라이플을 들고 있으면서 사격 가능한 상태라면
	if(weaponArray[0]==true)
	{
		if(curRifleAmmo>0)
		{
			// Clamp를 통한 탄약 수 차감
			curRifleAmmo = FMath::Clamp(curRifleAmmo-1, 0, 30);
			UE_LOG(LogTemp, Warning, TEXT("Cur Rifle Bullet : %d"), curRifleAmmo)
			FVector startLoc = FollowCamera->GetComponentLocation();
			FVector EndLoc = startLoc + FollowCamera->GetForwardVector()*10000.0f;
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // LineTrace로 히트 가능한 오브젝트 유형들.
			TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
			TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);
			TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
			TEnumAsByte<EObjectTypeQuery> PhysicsBody = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody);
			ObjectTypes.Add(WorldStatic);
			ObjectTypes.Add(WorldDynamic);
			ObjectTypes.Add(Pawn);
			ObjectTypes.Add(PhysicsBody);
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this); // LineTrace에서 제외할 대상
			FHitResult rifleHitResult;
			auto particleTrans = rifleComp->GetSocketTransform(FName("RifleFirePosition"));
			particleTrans.SetScale3D(FVector(0.7));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireParticle, particleTrans);
			FActorSpawnParameters param;
			param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			auto spawnTrans = rifleComp->GetSocketTransform(FName("BulletShell"));
			auto bulletShell = GetWorld()->SpawnActor<AActor>(BulletShellFactory, spawnTrans);
			bulletShell->SetLifeSpan(5.0f);
			auto bulSoundLoc = GetActorLocation()*FVector(0, 0, -80);
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), RifleBulletShellDropSound, bulSoundLoc, FRotator::ZeroRotator, 0.4, 1, 0);
			bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),startLoc, EndLoc, ObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::None, rifleHitResult, true);
			if(bHit)
			{
				auto randF = UKismetMathLibrary::RandomFloatInRange(-0.5, -0.8);
				auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.5, 0.5);
				AddControllerPitchInput(randF);
				AddControllerYawInput(randF2);
				FActorSpawnParameters params;
				params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto decalRot = UKismetMathLibrary::Conv_VectorToRotator(rifleHitResult.ImpactNormal);
				auto decalLoc = rifleHitResult.Location;
				auto decalTrans = UKismetMathLibrary::MakeTransform(decalLoc, decalRot);
				GetWorld()->SpawnActor<AActor>(ShotDecalFactory, decalTrans);
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, decalLoc, decalRot+FRotator(-90, 0, 0), FVector(0.5f));
				auto fireSocketLoc = rifleComp->GetSocketTransform(FName("RifleFirePosition")).GetLocation();
				// 탄 궤적 나이아가라 시스템 스폰
				UNiagaraComponent* niagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTrailSystem, rifleHitResult.Location, FRotator::ZeroRotator,FVector(1), true, true, ENCPoolMethod::AutoRelease);
				if(niagara)
				{
					// 나이아가라 파라미터 벡터 위치 변수 할당
					niagara->SetVectorParameter(FName("EndPoint"), fireSocketLoc);
				}
				CanShoot=false;
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), 1/BulletsPerSecRifle, false);
			}
			else
			{
				auto randF = UKismetMathLibrary::RandomFloatInRange(-0.5, -0.8);
				auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.5, 0.5);
				AddControllerPitchInput(randF);
				AddControllerYawInput(randF2);
				FVector niagaraSpawnLoc = FollowCamera->K2_GetComponentLocation();
				FVector ForwardLoc = niagaraSpawnLoc + FollowCamera->GetForwardVector()*10000.0f;
				auto FireLoc = rifleComp->GetSocketTransform(FName("RifleFirePosition")).GetLocation();
				UNiagaraComponent* niagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTrailSystem, ForwardLoc, FRotator::ZeroRotator, FVector(1), true, true, ENCPoolMethod::AutoRelease);
				if(niagara)
				{
					niagara->SetVectorParameter(FName("EndPoint"), FireLoc);
				}
				CanShoot=false;				
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), 1/BulletsPerSecRifle, false);
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
			curSniperAmmo = FMath::Clamp(curSniperAmmo-1, 0, 5);
			UE_LOG(LogTemp, Warning, TEXT("Cur Sniper Bullet : %d"), curSniperAmmo)
			FVector startLoc = FollowCamera->GetComponentLocation();
			FVector EndLoc = startLoc + FollowCamera->GetForwardVector()*10000.0f;
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // LineTrace로 히트 가능한 오브젝트 유형들.
			TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
			TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);
			TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
			TEnumAsByte<EObjectTypeQuery> PhysicsBody = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody);
			ObjectTypes.Add(WorldStatic);
			ObjectTypes.Add(WorldDynamic);
			ObjectTypes.Add(Pawn);
			ObjectTypes.Add(PhysicsBody);
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this); // LineTrace에서 제외할 대상
			FHitResult sniperHitResult;
			auto particleTrans = sniperComp->GetSocketTransform(FName("SniperFirePosition"));
			particleTrans.SetScale3D(FVector(0.7));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireParticle, particleTrans);
			FActorSpawnParameters param;
			param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			auto spawnTrans = sniperComp->GetSocketTransform(FName("BulletShell"));
			auto bulletShell = GetWorld()->SpawnActor<AActor>(BulletShellFactory, spawnTrans);
			bulletShell->SetLifeSpan(5.0f);
			auto bulSoundLoc = GetActorLocation()*FVector(0, 0, -80);
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), RifleBulletShellDropSound, bulSoundLoc, FRotator::ZeroRotator, 0.4, 1, 0);
			bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),startLoc, EndLoc, ObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::None, sniperHitResult, true);
			// 라인 트레이스가 적중했다면
			if(bHit)
			{
				auto randF = UKismetMathLibrary::RandomFloatInRange(-0.7, -1.2);
				auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.7, 0.8);
				AddControllerPitchInput(randF);
				AddControllerYawInput(randF2);
				FActorSpawnParameters params;
				params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto decalRot = UKismetMathLibrary::Conv_VectorToRotator(sniperHitResult.ImpactNormal);
				auto decalLoc = sniperHitResult.Location;
				auto decalTrans = UKismetMathLibrary::MakeTransform(decalLoc, decalRot);
				GetWorld()->SpawnActor<AActor>(ShotDecalFactory, decalTrans);
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, decalLoc, decalRot+FRotator(-90, 0, 0), FVector(0.5f));
				auto fireSocketLoc = sniperComp->GetSocketTransform(FName("SniperFirePosition")).GetLocation();
				// 탄 궤적 나이아가라 시스템 스폰
				UNiagaraComponent* niagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTrailSystem, sniperHitResult.Location, FRotator::ZeroRotator,FVector(1), true, true, ENCPoolMethod::AutoRelease);
				if(niagara)
				{
					// 나이아가라 파라미터 벡터 위치 변수 할당
					niagara->SetVectorParameter(FName("EndPoint"), fireSocketLoc);
				}
				CanShoot=false;
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), BulletsPerSecSniper, false);
			}
			// 라인 트레이스가 적중하지 않았다면
			else
			{
				auto randF = UKismetMathLibrary::RandomFloatInRange(-0.7, -1.2);
				auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.7, 0.8);
				AddControllerPitchInput(randF);
				AddControllerYawInput(randF2);
				FVector niagaraSpawnLoc = FollowCamera->K2_GetComponentLocation();
				FVector ForwardLoc = niagaraSpawnLoc + FollowCamera->GetForwardVector()*10000.0f;
				auto FireLoc = sniperComp->GetSocketTransform(FName("PistolFirePosition")).GetLocation();
				UNiagaraComponent* niagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTrailSystem, ForwardLoc, FRotator::ZeroRotator, FVector(1), true, true, ENCPoolMethod::AutoRelease);
				if(niagara)
				{
					niagara->SetVectorParameter(FName("EndPoint"), FireLoc);
				}
				CanShoot=false;				
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), BulletsPerSecSniper, false);
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
			curPistolAmmo = FMath::Clamp(curPistolAmmo-1, 0, 8);
			UE_LOG(LogTemp, Warning, TEXT("Cur Pistol Bullet : %d"), curPistolAmmo)
			PlayAnimMontage(zoomingMontage, 1, FName("PistolFire"));
			FVector startLoc = FollowCamera->GetComponentLocation();
			FVector EndLoc = startLoc + FollowCamera->GetForwardVector()*10000.0f;
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // LineTrace로 히트 가능한 오브젝트 유형들.
			TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
			TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);
			TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
			TEnumAsByte<EObjectTypeQuery> PhysicsBody = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody);
			ObjectTypes.Add(WorldStatic);
			ObjectTypes.Add(WorldDynamic);
			ObjectTypes.Add(Pawn);
			ObjectTypes.Add(PhysicsBody);
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this); // LineTrace에서 제외할 대상
			FHitResult pistolHitResult;
			auto particleTrans = pistolComp->GetSocketTransform(FName("PistolFirePosition"));
			particleTrans.SetScale3D(FVector(0.7));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireParticle, particleTrans);
			FActorSpawnParameters param;
			param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			auto spawnTrans = pistolComp->GetSocketTransform(FName("BulletShell"));
			auto bulletShell = GetWorld()->SpawnActor<AActor>(BulletShellFactory, spawnTrans);
			bulletShell->SetLifeSpan(5.0f);
			auto bulSoundLoc = GetActorLocation()*FVector(0, 0, -80);
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), RifleBulletShellDropSound, bulSoundLoc, FRotator::ZeroRotator, 0.4, 1, 0);
			bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),startLoc, EndLoc, ObjectTypes, true, ActorsToIgnore, EDrawDebugTrace::None, pistolHitResult, true);
			if(bHit)
			{
				auto randF = UKismetMathLibrary::RandomFloatInRange(-0.7, -1.2);
				auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.7, 0.8);
				AddControllerPitchInput(randF);
				AddControllerYawInput(randF2);
				FActorSpawnParameters params;
				params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto decalRot = UKismetMathLibrary::Conv_VectorToRotator(pistolHitResult.ImpactNormal);
				auto decalLoc = pistolHitResult.Location;
				auto decalTrans = UKismetMathLibrary::MakeTransform(decalLoc, decalRot);
				GetWorld()->SpawnActor<AActor>(ShotDecalFactory, decalTrans);
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletMarksParticle, decalLoc, decalRot+FRotator(-90, 0, 0), FVector(0.5f));
				auto fireSocketLoc = pistolComp->GetSocketTransform(FName("PistolFirePosition")).GetLocation();
				// 탄 궤적 나이아가라 시스템 스폰
				UNiagaraComponent* niagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTrailSystem, pistolHitResult.Location, FRotator::ZeroRotator,FVector(1), true, true, ENCPoolMethod::AutoRelease);
				if(niagara)
				{
					// 나이아가라 파라미터 벡터 위치 변수 할당
					niagara->SetVectorParameter(FName("EndPoint"), fireSocketLoc);
				}
				CanShoot=false;
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), 1/BulletsPerSecPistol, false);
			}
			else
			{
				auto randF = UKismetMathLibrary::RandomFloatInRange(-0.7, -1.2);
				auto randF2 = UKismetMathLibrary::RandomFloatInRange(-0.7, 0.8);
				AddControllerPitchInput(randF);
				AddControllerYawInput(randF2);
				FVector niagaraSpawnLoc = FollowCamera->K2_GetComponentLocation();
				FVector ForwardLoc = niagaraSpawnLoc + FollowCamera->GetForwardVector()*10000.0f;
				auto FireLoc = pistolComp->GetSocketTransform(FName("PistolFirePosition")).GetLocation();
				UNiagaraComponent* niagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTrailSystem, ForwardLoc, FRotator::ZeroRotator, FVector(1), true, true, ENCPoolMethod::AutoRelease);
				if(niagara)
				{
					niagara->SetVectorParameter(FName("EndPoint"), FireLoc);
				}
				CanShoot=false;				
				GetWorldTimerManager().SetTimer(shootEnableHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					CanShoot=true;
				}), 1/BulletsPerSecPistol, false);
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

