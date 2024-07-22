// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSelection.h"

#include "Eclipse/Game/EclipseGameInstance.h"
#include "Eclipse/Player/EclipsePlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"

void ULevelSelection::NativeConstruct()
{
    Super::NativeConstruct();

    // 레벨 선택 시작 애니메이션을 재생합니다.
    PlayAnimation(LevelSelectionStartAnim);

    // 플레이어 캐릭터, 플레이어 컨트롤러, 게임 인스턴스를 캐스팅합니다.
    player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    pc = Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());
    gi = Cast<UEclipseGameInstance>(GetWorld()->GetGameInstance());
    gi->IsWidgetOn = true;  // 위젯이 활성화되었음을 나타냅니다.

    // 레벨 선택 위젯 스위처의 활성화된 위젯 인덱스를 0으로 설정합니다.
    WidgetSwitcher_Level->SetActiveWidgetIndex(0);

    // quitBool 변수를 초기화합니다.
    quitBool = false;
}

void ULevelSelection::Level1Y()
{
    // 격리된 우주선으로 이동합니다.
    if (quitBool == false)
    {
        quitBool = true;  // quitBool 변수를 true로 설정하여 중복 실행을 방지합니다.
        UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);  // 게임 전용 입력 모드로 설정합니다.
        pc->SetShowMouseCursor(false);  // 마우스 커서를 숨깁니다.
        this->RemoveFromParent();  // 현재 위젯을 부모에서 제거합니다.
        player->MoveToIsolatedShip();  // 플레이어를 격리된 우주선으로 이동시킵니다.
    }
}

void ULevelSelection::Level1N()
{
    // 레벨 선택을 취소합니다.
    UGameplayStatics::PlaySound2D(GetWorld(), QuitSound);  // 취소 소리를 재생합니다.
    WidgetSwitcher_Level->SetActiveWidgetIndex(0);  // 레벨 선택 위젯 스위처의 활성화된 위젯 인덱스를 0으로 설정합니다.
    PlayAnimation(LevelSelectionStartAnim);  // 레벨 선택 시작 애니메이션을 재생합니다.
}

void ULevelSelection::Level2N()
{
    // 레벨 선택을 취소합니다.
    UGameplayStatics::PlaySound2D(GetWorld(), QuitSound);  // 취소 소리를 재생합니다.
    WidgetSwitcher_Level->SetActiveWidgetIndex(0);  // 레벨 선택 위젯 스위처의 활성화된 위젯 인덱스를 0으로 설정합니다.
    PlayAnimation(LevelSelectionStartAnim);  // 레벨 선택 시작 애니메이션을 재생합니다.
    FTimerHandle CursorHandle;
    GetWorld()->GetTimerManager().SetTimer(CursorHandle, FTimerDelegate::CreateLambda([this]()-> void
    {
        pc->SetShowMouseCursor(true);  // 마우스 커서를 다시 표시합니다.
        UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(pc, this);  // 게임 및 UI 입력 모드로 설정합니다.
    }), 1.f, false);  // 1초 후에 실행합니다.
}

void ULevelSelection::Level2Y()
{
    // 차단된 교차로로 이동합니다.
    if (quitBool == false)
    {
        quitBool = true;  // quitBool 변수를 true로 설정하여 중복 실행을 방지합니다.
        UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);  // 게임 전용 입력 모드로 설정합니다.
        pc->SetShowMouseCursor(false);  // 마우스 커서를 숨깁니다.
        this->RemoveFromParent();  // 현재 위젯을 부모에서 제거합니다.
        player->MoveToBlockedIntersection();  // 플레이어를 차단된 교차로로 이동시킵니다.
    }   
}

void ULevelSelection::ShowSingleLevelInfoFunc()
{
    UGameplayStatics::PlaySound2D(GetWorld(), AskSound);  // 소리를 재생합니다.
    WidgetSwitcher_LevelSelect->SetActiveWidgetIndex(0);  // 단일 레벨 선택 위젯 스위처의 활성화된 위젯 인덱스를 0으로 설정합니다.
    PlayAnimation(SingleLevelStartAnim);  // 단일 레벨 시작 애니메이션을 재생합니다.
}

void ULevelSelection::ShowMultiLevelInfoFunc()
{
    UGameplayStatics::PlaySound2D(GetWorld(), AskSound);  // 소리를 재생합니다.
    WidgetSwitcher_LevelSelect->SetActiveWidgetIndex(1);  // 다중 레벨 선택 위젯 스위처의 활성화된 위젯 인덱스를 1로 설정합니다.
    PlayAnimation(MultiLevelStartAnim);  // 다중 레벨 시작 애니메이션을 재생합니다.
}

void ULevelSelection::SelectExitGame()
{
    UGameplayStatics::PlaySound2D(GetWorld(), CloseSound);  // 소리를 재생합니다.

    // 레벨 선택을 종료합니다.
    UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);  // 게임 전용 입력 모드로 설정합니다.
    pc->SetShowMouseCursor(false);  // 마우스 커서를 숨깁니다.
    gi->IsWidgetOn = false;  // 위젯이 비활성화되었음을 나타냅니다.
    this->RemoveFromParent();  // 현재 위젯을 부모에서 제거합니다.
}

void ULevelSelection::OpenMoveIsolatedShipSelection()
{
    PlayAnimation(LevelSelectionEndAnim);  // 레벨 선택 종료 애니메이션을 재생합니다.
    FTimerHandle endHandle;
    GetWorld()->GetTimerManager().SetTimer(endHandle, FTimerDelegate::CreateLambda([this]()-> void
    {
        UGameplayStatics::PlaySound2D(GetWorld(), AskSound);  // 소리를 재생합니다.
        WidgetSwitcher_Level->SetActiveWidgetIndex(1);  // 레벨 선택 위젯 스위처의 활성화된 위젯 인덱스를 1로 설정합니다.
        PlayAnimation(LevelSelectionStartAnim);  // 레벨 선택 시작 애니메이션을 재생합니다.
    }), 0.75f, false);  // 0.75초 후에 실행합니다.
}

void ULevelSelection::OpenDesertedRoadSelection()
{
    PlayAnimation(LevelSelectionEndAnim);  // 레벨 선택 종료 애니메이션을 재생합니다.
    FTimerHandle endHandle;
    GetWorld()->GetTimerManager().SetTimer(endHandle, FTimerDelegate::CreateLambda([this]()-> void
    {
        UGameplayStatics::PlaySound2D(GetWorld(), AskSound);  // 소리를 재생합니다.
        WidgetSwitcher_Level->SetActiveWidgetIndex(2);  // 레벨 선택 위젯 스위처의 활성화된 위젯 인덱스를 2로 설정합니다.
        PlayAnimation(LevelSelectionStartAnim);  // 레벨 선택 시작 애니메이션을 재생합니다.
    }), 0.75f, false);  // 0.75초 후에 실행합니다.
}
