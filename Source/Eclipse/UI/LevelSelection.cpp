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

	PlayAnimation(LevelSelectionStartAnim);

	player=Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	pc=Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());
	gi=Cast<UEclipseGameInstance>(GetWorld()->GetGameInstance());
	gi->IsWidgetOn=true;

	//SelectLevel1Yes->OnClicked.AddDynamic(this, &ULevelSelection::Level1Y);
	//SelectLevel1No->OnClicked.AddDynamic(this, &ULevelSelection::Level1N);

	//SelectLevel2Yes->OnClicked.AddDynamic(this, &ULevelSelection::Level2Y);
	//SelectLevel2No->OnClicked.AddDynamic(this, &ULevelSelection::Level2N);
	
	//ExitButton->OnClicked.AddDynamic(this, &ULevelSelection::SelectExitGame);

	//IsolatedShipButton->OnClicked.AddDynamic(this, &ULevelSelection::OpenMoveIsolatedShipSelection);

	WidgetSwitcher_Level->SetActiveWidgetIndex(0);

	
	quitBool=false;
	
}

void ULevelSelection::Level1Y()
{
	//Move to Isolation Ship
	if(quitBool==false)
	{
		quitBool=true;
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
		pc->SetShowMouseCursor(false);
		this->RemoveFromParent();
		player->MoveToIsolatedShip();
	}	
}

void ULevelSelection::Level1N()
{
	//Cancel Level Selection
	UGameplayStatics::PlaySound2D(GetWorld(), QuitSound);
	WidgetSwitcher_Level->SetActiveWidgetIndex(0);
	PlayAnimation(LevelSelectionStartAnim);
}

void ULevelSelection::Level2Y()
{
	//Move to Battle Field
}

void ULevelSelection::Level2N()
{
	//Cancel Level Selection
	UGameplayStatics::PlaySound2D(GetWorld(), QuitSound);
	WidgetSwitcher_Level->SetActiveWidgetIndex(0);
	PlayAnimation(LevelSelectionStartAnim);
}

void ULevelSelection::SelectExitGame()
{
	UGameplayStatics::PlaySound2D(GetWorld(), CloseSound);

	//Exit Level Selection
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
	pc->SetShowMouseCursor(false);
	gi->IsWidgetOn=false;
	this->RemoveFromParent();
}

void ULevelSelection::OpenMoveIsolatedShipSelection()
{
	PlayAnimation(LevelSelectionEndAnim);
	FTimerHandle endHandle;
	GetWorld()->GetTimerManager().SetTimer(endHandle, FTimerDelegate::CreateLambda([this]()->void
	{
		UGameplayStatics::PlaySound2D(GetWorld(), AskSound);
		WidgetSwitcher_Level->SetActiveWidgetIndex(1);
		PlayAnimation(LevelSelectionStartAnim);
	}), 0.75f, false);				
}
