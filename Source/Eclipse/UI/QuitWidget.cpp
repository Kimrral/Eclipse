// Fill out your copyright notice in the Description page of Project Settings.


#include "QuitWidget.h"

#include "Eclipse/Game/EclipseGameInstance.h"
#include "Eclipse/Player/EclipsePlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UQuitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimation(QuitWidgetStartAnim);

	pc = Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());
	gi = Cast<UEclipseGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		gi->IsWidgetOn = true;
	}
}

void UQuitWidget::QuitSelectYes()
{
	const TEnumAsByte<EQuitPreference::Type> Types = EQuitPreference::Quit;
	UKismetSystemLibrary::QuitGame(GetWorld(), pc, Types, false);
}

void UQuitWidget::QuitSelectNo()
{
	UGameplayStatics::PlaySound2D(GetWorld(), CloseSound);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
	pc->SetShowMouseCursor(false);
	gi->IsWidgetOn = false;
	this->RemoveFromParent();
}
