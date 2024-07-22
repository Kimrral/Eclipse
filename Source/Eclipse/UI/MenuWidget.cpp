// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/WidgetSwitcher.h"
#include "Eclipse/Character/PlayerCharacter.h"
#include "Eclipse/Player/EclipsePlayerController.h"
#include "Kismet/GameplayStatics.h"

void UMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 메뉴 시작 애니메이션을 재생합니다.
    PlayAnimation(MenuStart);

    // 질문 소리를 재생합니다.
    UGameplayStatics::PlaySound2D(GetWorld(), AskSound);

    // 플레이어 캐릭터, 플레이어 컨트롤러, 게임 인스턴스를 캐스팅합니다.
    player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    pc = Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());
    gi = Cast<UEclipseGameInstance>(GetWorld()->GetGameInstance());
    gi->IsWidgetOn = true; // 위젯이 활성화되었음을 나타냅니다.

    // 메뉴 위젯 스위처의 활성화된 위젯 인덱스를 0으로 설정합니다.
    WidgetSwitcher_Menu->SetActiveWidgetIndex(0);

    // quitBool 변수를 초기화합니다.
    quitBool = false;
}

void UMenuWidget::ReturnToHideoutFunc()
{
    // 메뉴 종료 애니메이션을 재생합니다.
    PlayAnimation(MenuEnd);
    FTimerHandle EndHandle;
    // 타이머를 설정하여 일정 시간 후에 특정 기능을 실행합니다.
    GetWorld()->GetTimerManager().SetTimer(EndHandle, FTimerDelegate::CreateLambda([this]()-> void
    {
        // 질문 소리를 재생합니다.
        UGameplayStatics::PlaySound2D(GetWorld(), AskSound);
        // 메뉴 위젯 스위처의 활성화된 위젯 인덱스를 1로 설정합니다.
        WidgetSwitcher_Menu->SetActiveWidgetIndex(1);
        // 은신처 선택 시작 애니메이션을 재생합니다.
        PlayAnimation(HideOutSelectionStart);
    }), 0.75f, false); // 0.75초 후에 실행합니다.
}

void UMenuWidget::ReturnToHideoutYesFunc()
{
    if (quitBool == false)
    {
        quitBool = true; // quitBool 변수를 true로 설정하여 중복 실행을 방지합니다.
        // 레벨 선택 종료
        UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc); // 게임 전용 입력 모드로 설정합니다.
        pc->SetShowMouseCursor(false); // 마우스 커서를 숨깁니다.
        gi->IsWidgetOn = false; // 위젯이 비활성화되었음을 나타냅니다.

        // 플레이어 카메라 매니저를 가져와서 카메라 페이드를 시작합니다.
        APlayerCameraManager* PlayerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
        PlayerCam->StartCameraFade(0, 1, 2.0, FLinearColor::Black, false, true);
        FTimerHandle TimerHandle;
        // 타이머를 설정하여 2초 후에 플레이어를 은신처로 이동시킵니다.
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void
        {
            player->MoveToHideout(true);
        }), 2.f, false);
        
        // 현재 위젯을 부모에서 제거합니다.
        this->RemoveFromParent();
    }
}

void UMenuWidget::ReturnToHideoutNoFunc()
{
    // 위젯을 닫는 기능을 호출합니다.
    CloseWidgetFunc();
}

void UMenuWidget::SelectExitGameFunc()
{
    // 메뉴 종료 애니메이션을 재생합니다.
    PlayAnimation(MenuEnd);
    FTimerHandle EndHandle;
    // 타이머를 설정하여 일정 시간 후에 특정 기능을 실행합니다.
    GetWorld()->GetTimerManager().SetTimer(EndHandle, FTimerDelegate::CreateLambda([this]()-> void
    {
        // 질문 소리를 재생합니다.
        UGameplayStatics::PlaySound2D(GetWorld(), AskSound);
        // 메뉴 위젯 스위처의 활성화된 위젯 인덱스를 2로 설정합니다.
        WidgetSwitcher_Menu->SetActiveWidgetIndex(2);
        // 종료 선택 시작 애니메이션을 재생합니다.
        PlayAnimation(QuitSelectionStart);
    }), 0.75f, false); // 0.75초 후에 실행합니다.
}

void UMenuWidget::SelectExitGameYesFunc()
{
    // 게임 종료 타입을 설정합니다.
    const TEnumAsByte<EQuitPreference::Type> Types = EQuitPreference::Quit;
    // 게임을 종료합니다.
    UKismetSystemLibrary::QuitGame(GetWorld(), pc, Types, false);
}

void UMenuWidget::SelectExitGameNoFunc()
{
    // 위젯을 닫는 기능을 호출합니다.
    CloseWidgetFunc();
}

void UMenuWidget::ShowKeyGuide()
{
    // 메뉴 종료 애니메이션을 재생합니다.
    PlayAnimation(MenuEnd);
    FTimerHandle EndHandle;
    // 타이머를 설정하여 일정 시간 후에 특정 기능을 실행합니다.
    GetWorld()->GetTimerManager().SetTimer(EndHandle, FTimerDelegate::CreateLambda([this]()-> void
    {
        // 질문 소리를 재생합니다.
        UGameplayStatics::PlaySound2D(GetWorld(), AskSound);
        // 메뉴 위젯 스위처의 활성화된 위젯 인덱스를 3으로 설정합니다.
        WidgetSwitcher_Menu->SetActiveWidgetIndex(3);
        // 가이드 시작 애니메이션을 재생합니다.
        PlayAnimation(GuideStart);
    }), 0.75f, false); // 0.75초 후에 실행합니다.
}

void UMenuWidget::CloseWidgetFunc()
{
    // 닫기 소리를 재생합니다.
    UGameplayStatics::PlaySound2D(GetWorld(), CloseSound);

    // 레벨 선택 종료
    UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc); // 게임 전용 입력 모드로 설정합니다.
    pc->SetShowMouseCursor(false); // 마우스 커서를 숨깁니다.
    gi->IsWidgetOn = false; // 위젯이 비활성화되었음을 나타냅니다.
    // 현재 위젯을 부모에서 제거합니다.
    this->RemoveFromParent();
}
