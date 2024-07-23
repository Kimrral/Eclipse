// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/Enemy/Shadow.h"

#include "Eclipse/AI/EnemyFSM.h"
#include "Kismet/KismetMathLibrary.h"

AShadow::AShadow()
{
	// 적 FSM 초기화
	EnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("EnemyFSM"));
	
	// 무기 컴포넌트 초기화 및 메쉬에 부착
	WeaponComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponComp"));
	WeaponComp->SetupAttachment(GetMesh(), FName("hand_r")); // 'hand_r' 소켓에 부착
}

void AShadow::BeginPlay()
{
	Super::BeginPlay();

	// 무기 컴포넌트의 첫 번째 머티리얼을 가져옴
	const auto Material = WeaponComp->GetMaterial(0);
	
	// 동적 머티리얼 인스턴스 생성
	if(UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this))
	{
		// 무기 컴포넌트에 동적 머티리얼 설정
		WeaponComp->SetMaterial(0, DynamicMaterial);
	}
}

void AShadow::SetDissolveValue(const float Value)
{
	Super::SetDissolveValue(Value);
	
	// 0에서 1 사이의 값을 보간하여 설정
	const double Lerp = UKismetMathLibrary::Lerp(0, 1, Value);
	
	// 무기 컴포넌트의 첫 번째 머티리얼을 동적 머티리얼로 캐스팅
	UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(WeaponComp->GetMaterial(0));
	
	// 동적 머티리얼의 DissolveParams 파라미터 값을 설정
	DynamicMaterial->SetScalarParameterValue("DissolveParams", Lerp);
}
