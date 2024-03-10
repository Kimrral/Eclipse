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

	//QuitYes->OnClicked.AddDynamic(this, &UQuitWidget::QuitSelectYes);
	//QuitNo->OnClicked.AddDynamic(this, &UQuitWidget::QuitSelectNo);
	
	quitBool=false;

	PlayAnimation(QuitWidgetStartAnim);

}

void UQuitWidget::QuitSelectYes()
{
	TEnumAsByte<EQuitPreference::Type> types = EQuitPreference::Quit;
	UKismetSystemLibrary::QuitGame(GetWorld(),pc, types, false);
}

void UQuitWidget::QuitSelectNo()
{
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
	pc->SetShowMouseCursor(false);
	this->RemoveFromParent();
}

