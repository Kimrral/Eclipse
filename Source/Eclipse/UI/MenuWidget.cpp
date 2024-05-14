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

	PlayAnimation(MenuStart);

	UGameplayStatics::PlaySound2D(GetWorld(), AskSound);

	player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	pc = Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());
	gi = Cast<UEclipseGameInstance>(GetWorld()->GetGameInstance());
	gi->IsWidgetOn = true;

	WidgetSwitcher_Menu->SetActiveWidgetIndex(0);

	quitBool = false;
}

void UMenuWidget::ReturnToHideoutFunc()
{
	PlayAnimation(MenuEnd);
	FTimerHandle EndHandle;
	GetWorld()->GetTimerManager().SetTimer(EndHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		UGameplayStatics::PlaySound2D(GetWorld(), AskSound);
		WidgetSwitcher_Menu->SetActiveWidgetIndex(1);
		PlayAnimation(HideOutSelectionStart);
	}), 0.75f, false);
}

void UMenuWidget::ReturnToHideoutYesFunc()
{
	if (quitBool == false)
	{
		quitBool = true;
		//Exit Level Selection
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
		pc->SetShowMouseCursor(false);
		gi->IsWidgetOn = false;
		
		APlayerCameraManager* PlayerCam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		PlayerCam->StartCameraFade(0, 1, 2.0, FLinearColor::Black, false, true);
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()-> void
		{
			player->MoveToHideout(true);
		}), 2.f, false);
		
		this->RemoveFromParent();
	}
}

void UMenuWidget::ReturnToHideoutNoFunc()
{
	//Cancel Level Selection
	UGameplayStatics::PlaySound2D(GetWorld(), QuitSound);
	WidgetSwitcher_Menu->SetActiveWidgetIndex(0);
	PlayAnimation(MenuStart);
	FTimerHandle CursorHandle;
	GetWorld()->GetTimerManager().SetTimer(CursorHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		pc->SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(pc, this);
	}), 1.f, false);
}

void UMenuWidget::SelectExitGameFunc()
{
	PlayAnimation(MenuEnd);
	FTimerHandle EndHandle;
	GetWorld()->GetTimerManager().SetTimer(EndHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		UGameplayStatics::PlaySound2D(GetWorld(), AskSound);
		WidgetSwitcher_Menu->SetActiveWidgetIndex(2);
		PlayAnimation(QuitSelectionStart);
	}), 0.75f, false);
}

void UMenuWidget::SelectExitGameYesFunc()
{
	const TEnumAsByte<EQuitPreference::Type> Types = EQuitPreference::Quit;
	UKismetSystemLibrary::QuitGame(GetWorld(), pc, Types, false);
}

void UMenuWidget::SelectExitGameNoFunc()
{
	//Cancel Level Selection
	UGameplayStatics::PlaySound2D(GetWorld(), QuitSound);
	WidgetSwitcher_Menu->SetActiveWidgetIndex(0);
	PlayAnimation(MenuStart);
	FTimerHandle CursorHandle;
	GetWorld()->GetTimerManager().SetTimer(CursorHandle, FTimerDelegate::CreateLambda([this]()-> void
	{
		pc->SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(pc, this);
	}), 1.f, false);
}

void UMenuWidget::CloseWidgetFunc()
{
	UGameplayStatics::PlaySound2D(GetWorld(), CloseSound);

	//Exit Level Selection
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
	pc->SetShowMouseCursor(false);
	gi->IsWidgetOn = false;
	this->RemoveFromParent();
}
