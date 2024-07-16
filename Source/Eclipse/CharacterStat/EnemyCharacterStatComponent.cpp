// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/CharacterStat/EnemyCharacterStatComponent.h"

#include "Eclipse/Character/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// 이 컴포넌트의 기본 속성 값을 설정합니다.
UEnemyCharacterStatComponent::UEnemyCharacterStatComponent()
{
    // 이 컴포넌트가 게임 시작 시 초기화되도록 설정하고, 매 프레임마다 Tick() 함수를 호출하도록 설정합니다.
    // 필요하지 않은 경우 이러한 기능을 끄면 성능이 향상될 수 있습니다.
    PrimaryComponentTick.bCanEverTick = true;
    bWantsInitializeComponent = true;
}

void UEnemyCharacterStatComponent::InitializeComponent()
{
    // 부모 클래스의 InitializeComponent를 호출하여 기본 초기화를 수행합니다.
    Super::InitializeComponent();

    // 방어막 관련 부울 값을 초기화합니다.
    InitShieldBoolean();

    // 최대 체력 값으로 현재 체력을 설정합니다.
    SetHp(MaxEnemyHp);

    // 최대 방어막 값으로 현재 방어막을 설정합니다.
    SetShield(MaxEnemyShield);

    // 이 컴포넌트가 네트워크에서 복제되도록 설정합니다.
    SetIsReplicated(true);
}

float UEnemyCharacterStatComponent::ApplyDamage(float InDamage, AActor* DamageCauser)
{
    // 적이 피해를 입었을 때 호출되는 함수입니다.

    // 적이 피해를 입었다는 이벤트를 브로드캐스트합니다.
    OnEnemyDamaged.Broadcast();

    // 방어막이 파손되지 않은 경우 방어막에 피해를 입히고 체력을 반환합니다.
    if (!IsShieldBroken)
    {
        const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);
        ApplyShieldDamage(InDamage * 0.05f, DamageCauser);
        return ActualDamage;
    }

    // 방어막이 파손된 상태에서 적이 기절 상태이면 피해를 두 배로 증가시킵니다.
    if (IsStunned)
    {
        InDamage *= 2;
    }

    const float PrevHp = CurrentEnemyHp;
    const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

    // 새로운 체력 값을 설정합니다.
    SetHp(PrevHp - ActualDamage);

    // 체력이 0 이하로 떨어졌을 때 적이 죽었다는 이벤트를 브로드캐스트하고, 
    // 적을 처치한 플레이어에게 OnEnemyKill()을 호출합니다.
    if (CurrentEnemyHp <= 0.0f)
    {
        OnHpZero.Broadcast();
        PlayerCharacter = Cast<APlayerCharacter>(DamageCauser);
        if (PlayerCharacter)
        {
            PlayerCharacter->OnEnemyKill();
        }
    }    

    return ActualDamage;
}

float UEnemyCharacterStatComponent::ApplyShieldDamage(const float InShieldDamage, AActor* DamageCauser)
{
    // 방어막에 피해를 입히는 함수입니다.

    const float PrevShield = CurrentEnemyShield;
    const float ActualShieldDamage = FMath::Clamp<float>(InShieldDamage, 0, InShieldDamage);

    // 새로운 방어막 값을 설정합니다.
    SetShield(PrevShield - ActualShieldDamage);

    // 방어막이 0 이하로 떨어졌을 때 방어막이 파손되었다는 이벤트를 브로드캐스트합니다.
    if (CurrentEnemyShield <= 0.0f)
    {
        OnShieldZero.Broadcast();
    }    

    return ActualShieldDamage;
}

void UEnemyCharacterStatComponent::ReadyForReplication()
{
    // 부모 클래스의 ReadyForReplication을 호출하여 복제 준비를 수행합니다.
    Super::ReadyForReplication();
}

void UEnemyCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    // 부모 클래스의 GetLifetimeReplicatedProps를 호출하여 복제할 속성을 설정합니다.
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 복제할 속성들을 추가합니다.
    DOREPLIFETIME(UEnemyCharacterStatComponent, CurrentEnemyHp);
    DOREPLIFETIME(UEnemyCharacterStatComponent, MaxEnemyHp);
    DOREPLIFETIME(UEnemyCharacterStatComponent, CurrentEnemyShield);
    DOREPLIFETIME(UEnemyCharacterStatComponent, MaxEnemyShield);
    DOREPLIFETIME(UEnemyCharacterStatComponent, IsShieldBroken);
    DOREPLIFETIME(UEnemyCharacterStatComponent, IsStunned);
}

void UEnemyCharacterStatComponent::InitShieldBoolean()
{
    // 방어막의 최대 값이 0이면 방어막이 파손된 것으로 간주합니다.
    if (GetMaxShield() == 0.0f)
    {
        IsShieldBroken = true;
    }
    else
    {
        IsShieldBroken = false;
    }
}

void UEnemyCharacterStatComponent::SetHp(const float NewHp)
{
    // 현재 체력을 새 체력 값으로 설정하고 체력 변경 이벤트를 브로드캐스트합니다.
    CurrentEnemyHp = FMath::Clamp<float>(NewHp, 0.0f, MaxEnemyHp);
    OnHpChanged.Broadcast(GetCurrentHp(), GetMaxHp());
}

void UEnemyCharacterStatComponent::SetShield(const float NewShield)
{
    // 현재 방어막을 새 방어막 값으로 설정하고 방어막 변경 이벤트를 브로드캐스트합니다.
    CurrentEnemyShield = FMath::Clamp<float>(NewShield, 0.0f, MaxEnemyShield);
    OnShieldChanged.Broadcast(GetCurrentShield(), GetMaxShield());
}

void UEnemyCharacterStatComponent::OnRep_CurrentEnemyHp() const
{
    // 체력이 복제되었을 때 호출되는 함수입니다.
    OnHpChanged.Broadcast(GetCurrentHp(), GetMaxHp());

    // 체력이 0 이하로 떨어졌을 때 체력 0 이벤트를 브로드캐스트합니다.
    if (CurrentEnemyHp <= 0.0f)
    {
        OnHpZero.Broadcast();
    }
}

void UEnemyCharacterStatComponent::OnRep_MaxEnemyHp() const
{
    // 최대 체력이 복제되었을 때 호출되는 함수입니다.
    OnHpChanged.Broadcast(GetCurrentHp(), GetMaxHp());
}

void UEnemyCharacterStatComponent::OnRep_CurrentEnemyShield() const
{
    // 방어막이 복제되었을 때 호출되는 함수입니다.
    OnShieldChanged.Broadcast(GetCurrentShield(), GetMaxShield());

    // 방어막이 0 이하로 떨어졌을 때 방어막 0 이벤트를 브로드캐스트합니다.
    if (CurrentEnemyShield <= 0.0f)
    {
        OnShieldZero.Broadcast();
    }
}

void UEnemyCharacterStatComponent::OnRep_MaxEnemyShield() const
{
    // 최대 방어막이 복제되었을 때 호출되는 함수입니다.
    OnShieldChanged.Broadcast(GetCurrentShield(), GetMaxShield());
}
