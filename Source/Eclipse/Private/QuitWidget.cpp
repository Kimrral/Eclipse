// Fill out your copyright notice in the Description page of Project Settings.


#include "QuitWidget.h"

#include "EclipsePlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UQuitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	pc=Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());

	QuitYes->OnClicked.AddDynamic(this, &UQuitWidget::QuitY);
	QuitNo->OnClicked.AddDynamic(this, &UQuitWidget::QuitN);


	quitBool=false;
}

void UQuitWidget::QuitY()
{
	TEnumAsByte<EQuitPreference::Type> types = EQuitPreference::Quit;
	UKismetSystemLibrary::QuitGame(GetWorld(),pc, types, false);
}

void UQuitWidget::QuitN()
{
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
	pc->SetShowMouseCursor(false);
	this->RemoveFromParent();
}

