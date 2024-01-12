// Fill out your copyright notice in the Description page of Project Settings.


#include "QuitWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UQuitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	QuitYes->OnClicked.AddDynamic(this, &UQuitWidget::QuitY);
	QuitNo->OnClicked.AddDynamic(this, &UQuitWidget::QuitN);
}

void UQuitWidget::QuitY()
{
	auto pc = GetWorld()->GetFirstPlayerController();
	TEnumAsByte<EQuitPreference::Type> types = EQuitPreference::Quit;
	UKismetSystemLibrary::QuitGame(GetWorld(),pc, types, false);
}

void UQuitWidget::QuitN()
{
	auto pc = GetWorld()->GetFirstPlayerController();
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
	pc->SetShowMouseCursor(false);
	this->RemoveFromParent();
}
