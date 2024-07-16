// Copyright Epic Games, Inc. All Rights Reserved.

#include "EclipseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "EclipseGameState.h"
#include "Eclipse/Character/PlayerCharacter.h"

AEclipseGameMode::AEclipseGameMode()
{
    // 게임 상태 클래스를 설정합니다. 이 클래스는 게임의 상태를 관리합니다.
    GameStateClass = AEclipseGameState::StaticClass();
}

// 첫 실행 시 스폰될 플레이어 스타트 지점을 선택합니다. 이 함수는 AGameModeBase의 기본 구현을 오버라이딩합니다.
AActor* AEclipseGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    // 플레이어 스타트 액터를 저장할 배열을 정의합니다.
    TArray<class AActor*> OutActors;
    // 'Hideout' 태그가 붙은 플레이어 스타트 액터를 저장할 배열을 정의합니다.
    TArray<class APlayerStart*> TargetPlayerStarts;

    // 게임 월드에서 플레이어 스타트 액터를 모두 가져옵니다.
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), playerStartFactory, OutActors);

    // 가져온 플레이어 스타트 액터들에 대해 반복합니다.
    for (const auto PlayerStarts : OutActors)
    {
        // 플레이어 스타트 액터로 캐스팅합니다.
        if (const auto PlayerStart = Cast<APlayerStart>(PlayerStarts))
        {
            if (PlayerStart && PlayerStart->PlayerStartTag == FName("Hideout"))
            {
                // 플레이어 스타트 위치의 중앙 좌표를 가져옵니다.
                const FVector Center = PlayerStart->GetActorLocation();
                // 충돌 쿼리 파라미터를 정의합니다.
                FCollisionQueryParams Params;
                bool AlreadyLocated = false;
                
                // 충돌 영역을 설정하여 플레이어 캐릭터가 존재하는지 확인합니다.
                if (TArray<FOverlapResult> HitObj; GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(50), Params))
                {
                    // 충돌 결과에서 플레이어 캐릭터가 있는지 확인합니다.
                    for (int i = 0; i < HitObj.Num(); ++i)
                    {
                        if (Cast<APlayerCharacter>(HitObj[i].GetActor()))
                        {
                            // 플레이어 캐릭터가 이미 위치해 있으면 해당 플레이어 스타트를 건너뜁니다.
                            AlreadyLocated = true;
                            break;
                        }                        
                    }
                }
                // 이미 위치해 있는 경우, 다음 플레이어 스타트로 넘어갑니다.
                if (AlreadyLocated)
                {
                    continue;
                }
                // 유효한 플레이어 스타트 액터를 타겟 배열에 추가합니다.
                TargetPlayerStarts.Add(PlayerStart);
            }
        }
    }
    
    // 타겟 플레이어 스타트 배열에서 랜덤으로 플레이어 스타트를 선택합니다.
    if (const auto PlayerStartRandIndex = FMath::RandRange(0, TargetPlayerStarts.Num() - 1); TargetPlayerStarts.IsValidIndex(PlayerStartRandIndex))
    {
        // 선택된 플레이어 스타트를 반환합니다.
        APlayerStart* const TargetPlayerStart = TargetPlayerStarts[PlayerStartRandIndex];
        return TargetPlayerStart;
    }
    
    // 적절한 플레이어 스타트가 없는 경우, nullptr을 반환합니다.
    return nullptr;
}
