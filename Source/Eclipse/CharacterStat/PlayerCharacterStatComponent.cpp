// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterStatComponent.h"

#include "Eclipse/Player/EclipsePlayerController.h"
#include "Net/UnrealNetwork.h"

// 이 컴포넌트의 기본 속성 값을 설정합니다.
UPlayerCharacterStatComponent::UPlayerCharacterStatComponent()
{
    // 이 컴포넌트가 초기화되도록 설정합니다.
    bWantsInitializeComponent = true;
}

void UPlayerCharacterStatComponent::InitializeComponent()
{
    // 부모 클래스의 InitializeComponent를 호출하여 기본 초기화를 수행합니다.
    Super::InitializeComponent();

    // 최대 체력 값으로 현재 체력을 설정합니다.
    SetHp(MaxHp);

    // 기본적으로 100000의 루블을 추가합니다.
    AddRouble(100000.f);

    // 이 컴포넌트가 네트워크에서 복제되도록 설정합니다.
    SetIsReplicated(true);
}

float UPlayerCharacterStatComponent::ApplyDamage(const float InDamage, AActor* DamageCauser)
{
    // 체력을 감소시키고, 체력이 0 이하로 떨어지면 플레이어가 죽었다고 처리합니다.

    const float PrevHp = CurrentHp;
    const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

    // 새로운 체력 값을 설정합니다.
    SetHp(PrevHp - ActualDamage);
    
    // 체력이 0 이하로 떨어졌을 때 플레이어가 죽었다고 처리합니다.
    if (CurrentHp <= 0.0f)
    {
        PlayerCharacter = Cast<APlayerCharacter>(DamageCauser);
        if (PlayerCharacter)
        {
            PlayerCharacter->OnPlayerKill(); // 플레이어가 적을 처치했다고 처리합니다.
        }
        OnHpZero.Broadcast(); // 체력이 0이 되었다는 이벤트를 브로드캐스트합니다.
    }
    return ActualDamage;
}

void UPlayerCharacterStatComponent::ReadyForReplication()
{
    // 부모 클래스의 ReadyForReplication을 호출하여 복제 준비를 수행합니다.
    Super::ReadyForReplication();
}

void UPlayerCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    // 부모 클래스의 GetLifetimeReplicatedProps를 호출하여 복제할 속성을 설정합니다.
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 복제할 속성들을 추가합니다.
    DOREPLIFETIME(UPlayerCharacterStatComponent, CurrentHp);
    DOREPLIFETIME(UPlayerCharacterStatComponent, MaxHp);    
    DOREPLIFETIME(UPlayerCharacterStatComponent, CurrentRouble);
    DOREPLIFETIME(UPlayerCharacterStatComponent, RecoilStatMultiplier);
    DOREPLIFETIME(UPlayerCharacterStatComponent, FireIntervalStatMultiplier);
    DOREPLIFETIME(UPlayerCharacterStatComponent, DamageStatMultiplier);
    DOREPLIFETIME(UPlayerCharacterStatComponent, AccumulatedDamageToPlayer);
    DOREPLIFETIME(UPlayerCharacterStatComponent, AccumulatedDamageToEnemy);
    DOREPLIFETIME(UPlayerCharacterStatComponent, AccumulatedDamageToBoss);
}

void UPlayerCharacterStatComponent::SetRecoilRate(const TArray<bool>& WeaponArray)
{
    // 주어진 무기 배열에 따라 반동 비율을 설정합니다.

    if (WeaponArray[0] == true) // 소총을 사용하는 경우
    {
        PitchRecoilRate = GenerateRandomPitchRecoilRate(BaseRiflePitchRecoilRate) * RecoilStatMultiplier;
        YawRecoilRate = GenerateRandomYawRecoilRate(BaseRifleYawRecoilRate) * RecoilStatMultiplier;
        return;
    }
    if (WeaponArray[1] == true) // 저격총을 사용하는 경우
    {
        PitchRecoilRate = GenerateRandomPitchRecoilRate(BaseSniperPitchRecoilRate) * RecoilStatMultiplier;
        YawRecoilRate = GenerateRandomYawRecoilRate(BaseSniperYawRecoilRate) * RecoilStatMultiplier;
        return;
    }
    if (WeaponArray[2] == true) // 권총을 사용하는 경우
    {
        PitchRecoilRate = GenerateRandomPitchRecoilRate(BasePistolPitchRecoilRate) * RecoilStatMultiplier;
        YawRecoilRate = GenerateRandomYawRecoilRate(BasePistolYawRecoilRate) * RecoilStatMultiplier;
        return;
    }
    if (WeaponArray[3] == true) // M249 기관총을 사용하는 경우
    {
        PitchRecoilRate = GenerateRandomPitchRecoilRate(BaseM249PitchRecoilRate) * RecoilStatMultiplier;
        YawRecoilRate = GenerateRandomYawRecoilRate(BaseM249YawRecoilRate) * RecoilStatMultiplier;
    }
}

int32 UPlayerCharacterStatComponent::GenerateRandomInteger(const float InFloat) const
{
    // 주어진 부동 소수점 값을 바탕으로 무작위 정수를 생성합니다.
    const double DoubleRandInteger = FMath::FRandRange(InFloat * 0.8, InFloat * 1.2);    
    return DoubleRandInteger;
}

void UPlayerCharacterStatComponent::SetHp(const float NewHp)
{
    // 현재 체력을 새 체력 값으로 설정하고 체력 변경 이벤트를 브로드캐스트합니다.
    CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);
    OnHpChanged.Broadcast(); // 체력 변경 이벤트를 브로드캐스트합니다.
}

void UPlayerCharacterStatComponent::SubtractMaxHp(const float SubtractMaxHpAmount)
{
    // 최대 체력을 줄이고, 현재 체력을 최대 체력보다 낮게 설정합니다.

    if(GetCurrentHp() > 100.f)
    {
        SetHp(GetCurrentHp() - (GetCurrentHp() - 100.f)); // 현재 체력을 100으로 설정합니다.
        MaxHp = FMath::Clamp(MaxHp - SubtractMaxHpAmount, 0, MaxHp - SubtractMaxHpAmount);
    }
    else
    {
        MaxHp = FMath::Clamp(MaxHp - SubtractMaxHpAmount, 0, MaxHp - SubtractMaxHpAmount);
    }
    
    OnHpChanged.Broadcast(); // 체력 변경 이벤트를 브로드캐스트합니다.
}

void UPlayerCharacterStatComponent::OnRep_CurrentHp() const
{
    // 체력이 복제되었을 때 호출되는 함수입니다.
    OnHpChanged.Broadcast(); // 체력 변경 이벤트를 브로드캐스트합니다.
    if (CurrentHp <= 0.0f)
    {
        OnHpZero.Broadcast(); // 체력이 0 이하로 떨어졌을 때 체력 0 이벤트를 브로드캐스트합니다.
    }
}

void UPlayerCharacterStatComponent::OnRep_MaxHp() const
{
    // 최대 체력이 복제되었을 때 호출되는 함수입니다.
    OnHpChanged.Broadcast(); // 체력 변경 이벤트를 브로드캐스트합니다.
}

float UPlayerCharacterStatComponent::GetAttackDamage(const TArray<bool>& WeaponArray, const bool IsPlayer) const
{
    // 무기 배열과 플레이어 여부에 따라 공격 피해를 계산합니다.

    if (WeaponArray[0] == true) // 소총을 사용하는 경우
    {
        if (IsPlayer)
        {
            return FMath::RoundHalfToEven(GenerateRandomInteger(PlayerAttackDamageRifle) * DamageStatMultiplier);
        }
        return FMath::RoundHalfToEven(GenerateRandomInteger(EnemyAttackDamageRifle) * DamageStatMultiplier);
    }
    if (WeaponArray[1] == true) // 저격총을 사용하는 경우
    {
        if (IsPlayer)
        {            
            return FMath::RoundHalfToEven(GenerateRandomInteger(PlayerAttackDamageSniper) * DamageStatMultiplier);
        }        
        return FMath::RoundHalfToEven(GenerateRandomInteger(EnemyAttackDamageSniper) * DamageStatMultiplier);
    }
    if (WeaponArray[2] == true) // 권총을 사용하는 경우
    {
        if (IsPlayer)
        {            
            return FMath::RoundHalfToEven(GenerateRandomInteger(PlayerAttackDamagePistol) * DamageStatMultiplier);
        }
        return FMath::RoundHalfToEven(GenerateRandomInteger(EnemyAttackDamagePistol) * DamageStatMultiplier);
    }
    if (WeaponArray[3] == true) // M249 기관총을 사용하는 경우
    {
        if (IsPlayer)
        {
            return FMath::RoundHalfToEven(GenerateRandomInteger(PlayerAttackDamageM249) * DamageStatMultiplier);
        }
        return FMath::RoundHalfToEven(GenerateRandomInteger(EnemyAttackDamageM249) * DamageStatMultiplier);
    }
    return 0; // 해당하는 무기가 없을 경우 0을 반환합니다.
}

float UPlayerCharacterStatComponent::GetFireInterval(const TArray<bool>& WeaponArray) const
{
    // 무기 배열에 따라 발사 간격을 반환합니다.

    if (WeaponArray[0] == true) // 소총을 사용하는 경우
    {
        return BulletsPerSecRifle * FireIntervalStatMultiplier;
    }
    if (WeaponArray[1] == true) // 저격총을 사용하는 경우
    {
        return BulletsPerSecSniper * FireIntervalStatMultiplier;
    }
    if (WeaponArray[2] == true) // 권총을 사용하는 경우
    {
        return BulletsPerSecPistol * FireIntervalStatMultiplier;
    }
    if (WeaponArray[3] == true) // M249 기관총을 사용하는 경우
    {
        return BulletsPerSecM249 * FireIntervalStatMultiplier;
    }
    return 0; // 해당하는 무기가 없을 경우 0을 반환합니다.
}
