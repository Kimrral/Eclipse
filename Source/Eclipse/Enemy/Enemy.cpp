// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Eclipse/Item/ArmorActor.h"
#include "Eclipse/Player/EclipsePlayerController.h"
#include "Eclipse/Animation/EnemyAnim.h"
#include "Eclipse/AI/EnemyFSM.h"
#include "Eclipse/Item/GoggleActor.h"
#include "GuardianProjectile.h"
#include "Eclipse/Item/HeadsetActor.h"
#include "Eclipse/Item/HelmetActor.h"
#include "Eclipse/Item/M249MagActor.h"
#include "Eclipse/Item/MaskActor.h"
#include "Eclipse/Item/PistolMagActor.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/Item/RifleMagActor.h"
#include "Eclipse/Item/SniperMagActor.h"
#include "Components/CapsuleComponent.h"
#include "Eclipse/AI/EclipseAIController.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/Game/EclipseGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Enemy FSM
	EnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("enemyFSM"));
	// Pawn Sensor
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	// Stat Component 
	EnemyStat = CreateDefaultSubobject<UEnemyCharacterStatComponent>(TEXT("Stat"));
	// AI Controller
	AIControllerClass = AEclipseAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);

	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::OnPawnDetected);

	EnemyStat->OnShieldZero.AddUObject(this, &AEnemy::OnShieldDestroy);

	EnemyAnim = Cast<UEnemyAnim>(GetMesh()->GetAnimInstance());
	gameMode = Cast<AEclipseGameMode>(GetWorld()->GetAuthGameMode());
	PC = Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());

	SetDissolveMaterial();

	// Timeline Binding
	if (DissolveCurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &AEnemy::SetDissolveValue);
		DissolveTimeline.AddInterpFloat(DissolveCurveFloat, TimelineProgress);
	}
}

void AEnemy::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DissolveTimeline.TickTimeline(DeltaSeconds);
}

void AEnemy::OnDie()
{
	StopAnimMontage();
	EnemyStat->IsStunned = false;
	EnemyFSM->Timeline.Stop();
	EnemyFSM->SetComponentTickEnabled(false);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCharacterMovement()->Deactivate();
	GetWorld()->GetTimerManager().ClearTimer(StunHandle);
	UCapsuleComponent* const Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorTickEnabled(true);
	OnDestroy();
}

void AEnemy::OnPawnDetected(APawn* Pawn)
{
	if (APlayerCharacter* DetectedPawn = Cast<APlayerCharacter>(Pawn))
	{
		if (EnemyFSM->player == nullptr)
		{
			EnemyFSM->player = DetectedPawn;
		}
	}
}

void AEnemy::Damaged(const int Damage, AActor* DamageCauser)
{
	DamagedRPCServer(Damage, DamageCauser);
	EnemyStat->ApplyDamage(Damage, DamageCauser);
}

void AEnemy::DamagedRPCServer_Implementation(const int Damage, AActor* DamageCauser)
{
	DamagedRPCMulticast(Damage, DamageCauser);
}

bool AEnemy::DamagedRPCServer_Validate(int Damage, AActor* DamageCauser)
{
	return true;
}

void AEnemy::DamagedRPCMulticast_Implementation(int Damage, AActor* DamageCauser)
{
	if (EnemyStat->IsShieldBroken)
	{
		FTimerHandle overlayMatHandle;
		GetMesh()->SetOverlayMaterial(HitOverlayMat);
		GetWorldTimerManager().ClearTimer(overlayMatHandle);
		GetWorldTimerManager().SetTimer(overlayMatHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			GetMesh()->SetOverlayMaterial(nullptr);
		}), 0.3f, false);
	}
	else
	{
		FTimerHandle overlayMatHandle;
		GetMesh()->SetOverlayMaterial(HitOverlayMatShield);
		GetWorldTimerManager().ClearTimer(overlayMatHandle);
		GetWorldTimerManager().SetTimer(overlayMatHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			GetMesh()->SetOverlayMaterial(nullptr);
		}), 0.3f, false);
	}
}

void AEnemy::OnShieldDestroy()
{
	EnemyStat->IsShieldBroken = true;
	EnemyStat->IsStunned = true;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), EnemyFSM->ShieldBreakSound, GetActorLocation(), FRotator::ZeroRotator);
	FTransform EmitterTrans = GetMesh()->GetSocketTransform(FName("ShieldSocket"));
	EmitterTrans.SetScale3D(FVector(4));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyFSM->ShieldBreakEmitter, EmitterTrans);
	// 움직임 즉시 중단
	GetCharacterMovement()->StopMovementImmediately();
	// Movement Mode = None [움직임 차단]
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	StopAnimMontage();
	PlayAnimMontage(stunMontage, 1, FName("StunStart"));
	GetWorld()->GetTimerManager().SetTimer(StunHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		EnemyStat->IsStunned = false;
		StopAnimMontage();
		// Movement Mode = Walking [움직임 재개]
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		// Shield 회복
		EnemyStat->SetShield(EnemyStat->GetMaxShield());
		EnemyStat->IsShieldBroken = false;
		//EnemyFSM->player->bossHPUI->shieldProgressBar->SetPercent(1);
		EnemyFSM->SetState(EEnemyState::MOVE);
	}), 7.0f, false);
}

void AEnemy::OnDestroy()
{
	if (HasAuthority())
	{
		DropReward();
	}
	DissolveTimeline.PlayFromStart();
}

void AEnemy::DropReward()
{
	DropMagazine();
	DropGear();
}

void AEnemy::DropMagazine() const
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (const auto RandIndex = FMath::RandRange(0, 3); RandIndex == 0)
	{
		GetWorld()->SpawnActor<ARifleMagActor>(RifleMagActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (RandIndex == 1)
	{
		GetWorld()->SpawnActor<ASniperMagActor>(SniperMagActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (RandIndex == 2)
	{
		GetWorld()->SpawnActor<APistolMagActor>(PistolMagActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (RandIndex == 3)
	{
		GetWorld()->SpawnActor<AM249MagActor>(M249MagActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
}

void AEnemy::DropGear() const
{
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (const auto RandIndex = FMath::RandRange(0, 4); RandIndex == 0)
	{
		GetWorld()->SpawnActor<AHelmetActor>(HelmetActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (RandIndex == 1)
	{
		GetWorld()->SpawnActor<AHeadsetActor>(HeadsetActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (RandIndex == 2)
	{
		GetWorld()->SpawnActor<AMaskActor>(MaskActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (RandIndex == 3)
	{
		GetWorld()->SpawnActor<AGoggleActor>(GoggleActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
	else if (RandIndex == 4)
	{
		GetWorld()->SpawnActor<AArmorActor>(ArmorActorFactory, GetActorLocation(), GetActorRotation(), param);
	}
}

void AEnemy::FireProcess() const
{
}

void AEnemy::SetDissolveValue(const float Value)
{
	const double Lerp = UKismetMathLibrary::Lerp(0, 1, Value);
	for (const auto& DynamicMaterialIndex : DynamicMaterialIndices)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(DynamicMaterialIndex));
		DynamicMaterial->SetScalarParameterValue("DissolveParams", Lerp);
	}
	if (Lerp >= 1.f)
	{
		this->Destroy();
	}
}

void AEnemy::SetDissolveMaterial()
{
	if (USkeletalMeshComponent* SkeletalMeshComponent = GetMesh())
	{
		TArray<UMaterialInterface*> Materials = SkeletalMeshComponent->GetMaterials();
		uint32 MaterialIndex = 0;
		for (UMaterialInterface* const Material : Materials)
		{
			if (UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this))
			{
				SkeletalMeshComponent->SetMaterial(MaterialIndex, DynamicMaterial);
				DynamicMaterialIndices.Add(MaterialIndex);
			}
			++MaterialIndex;
		}
	}
}
