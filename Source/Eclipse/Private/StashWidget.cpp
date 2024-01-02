// Fill out your copyright notice in the Description page of Project Settings.


#include "StashWidget.h"

#include "EclipsePlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UStashWidget::NativeConstruct()
{
	Super::NativeConstruct();

	pc=Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());
	playerCPP=Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	ExitButton->OnPressed.AddDynamic(this, &UStashWidget::CloseStashWidget);
}

void UStashWidget::CloseStashWidget()
{
	if(IsInViewport()==true&&pc)
	{
		playerCPP->bStashWidgetOn=false;
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
		pc->SetShowMouseCursor(false);
		this->RemoveFromParent();
	}		
}
