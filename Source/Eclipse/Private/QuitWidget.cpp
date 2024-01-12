// Fill out your copyright notice in the Description page of Project Settings.


#include "QuitWidget.h"

#include "EclipsePlayerController.h"
#include "PlayerCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UQuitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	pc=Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());

	QuitYes->OnClicked.AddDynamic(this, &UQuitWidget::QuitY);
	QuitNo->OnClicked.AddDynamic(this, &UQuitWidget::QuitN);

	LevelYes->OnClicked.AddDynamic(this, &UQuitWidget::LevelY);
	LevelNo->OnClicked.AddDynamic(this, &UQuitWidget::LevelN);

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

void UQuitWidget::LevelY()
{
	if(quitBool==false)
	{
		quitBool=true;
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
		pc->SetShowMouseCursor(false);
		this->RemoveFromParent();
		player->MoveToIsolatedShip();
	}	
}

void UQuitWidget::LevelN()
{
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
	pc->SetShowMouseCursor(false);
	this->RemoveFromParent();
}
