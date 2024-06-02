// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Eclipse/Player/EclipsePlayerController.h"
#include "Eclipse/Animation/EnemyAnim.h"
#include "Eclipse/AI/EnemyFSM.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Eclipse/AI/EclipseAIController.h"
#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"
#include "Eclipse/Game/EclipseGameMode.h"
#include "Eclipse/Item/RewardManagerComponent.h"
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

	// Pawn Sensor
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	// Reward Manager
	RewardManager = CreateDefaultSubobject<URewardManagerComponent>(TEXT("RewardManagerComponent"));
	// Stat Component 
	EnemyStat = CreateDefaultSubobject<UEnemyCharacterStatComponent>(TEXT("Stat"));

	AEnemy::SetAIController();

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

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
	GameMode = Cast<AEclipseGameMode>(GetWorld()->GetAuthGameMode());
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
	if (::IsValid(EnemyFSM))
	{
		EnemyFSM->Timeline.Stop();
		EnemyFSM->SetComponentTickEnabled(false);
	}
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
		if (EnemyFSM && EnemyFSM->Player == nullptr)
		{
			EnemyFSM->Player = DetectedPawn;
		}
		bPlayerInSight = true;
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemy::ResetPawnDetection, 1.f, false);
	}
}

void AEnemy::ResetPawnDetection()
{
	bPlayerInSight = false;
}

void AEnemy::Damaged(const int Damage, AActor* DamageCauser)
{
	if(IsLocallyViewed()) SetDamagedOverlayMaterial();
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
	if (!HasAuthority())
	{
		if (!IsLocallyViewed())
		{
			SetDamagedOverlayMaterial();
		}
	}
}

void AEnemy::ResetOverlayMaterial() const
{
	GetMesh()->SetOverlayMaterial(nullptr);
}

void AEnemy::OnShieldDestroy()
{
	if (::IsValid(EnemyStat))
	{
		EnemyStat->IsShieldBroken = true;
		EnemyStat->IsStunned = true;

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShieldBreakSound, GetActorLocation(), FRotator::ZeroRotator);
		FTransform EmitterTrans = GetMesh()->GetSocketTransform(FName("ShieldSocket"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShieldBreakEmitter, EmitterTrans);
		EmitterTrans.SetScale3D(FVector(6));

		// 움직임 즉시 중단
		GetCharacterMovement()->StopMovementImmediately();
		// Movement Mode = None [움직임 차단]
		GetCharacterMovement()->SetMovementMode(MOVE_None);
		bUseControllerRotationYaw=false;
		StopAnimMontage();
		PlayAnimMontage(AnimMontage, 1, FName("StunStart"));
		GetWorld()->GetTimerManager().SetTimer(StunHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			EnemyStat->IsStunned = false;
			StopAnimMontage();
			// Movement Mode = Walking [움직임 재개]
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			bUseControllerRotationYaw=true;
			// Shield 회복
			EnemyStat->SetShield(EnemyStat->GetMaxShield());
			EnemyStat->IsShieldBroken = false;
			if (::IsValid(EnemyFSM)) EnemyFSM->SetState(EEnemyState::MOVE);
		}), 7.0f, false);
	}
}

void AEnemy::SetAIController()
{
	// AI Controller
	AIControllerClass = AEclipseAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


void AEnemy::OnDestroy()
{
	if (HasAuthority())
	{
		RewardManager->DropRewardServer(GetActorTransform());
	}
	DissolveTimeline.PlayFromStart();
}

void AEnemy::FireProcess_Implementation() const
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

void AEnemy::SetDamagedOverlayMaterial()
{
	if (EnemyStat->IsShieldBroken)
	{
		FTimerHandle OverlayMatHandle;
		GetMesh()->SetOverlayMaterial(HitOverlayMat);
		GetWorldTimerManager().ClearTimer(OverlayMatHandle);
		GetWorldTimerManager().SetTimer(OverlayMatHandle, this, &AEnemy::ResetOverlayMaterial, 0.1f, false);
	}
	else
	{
		FTimerHandle OverlayMatHandle;
		GetMesh()->SetOverlayMaterial(HitOverlayMatShield);
		GetWorldTimerManager().ClearTimer(OverlayMatHandle);
		GetWorldTimerManager().SetTimer(OverlayMatHandle, this, &AEnemy::ResetOverlayMaterial, 0.1f, false);
	}
}
