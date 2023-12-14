// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlot.h"

#include "DragThumbnail.h"
#include "EclipsePlayerController.h"
#include "TabHoveredInfoWidget.h"
#include "TabWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	pc=Cast<AEclipsePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));


	DragThumbnail=CreateWidget<UDragThumbnail>(GetWorld(), DragThumbnailFactory);
	TabWidget=CreateWidget<UTabWidget>(GetWorld(), TabWidgetFactory);
	TabHoveredInfoWidget=CreateWidget<UTabHoveredInfoWidget>(GetWorld(), TabHoveredInfoWidgetFactory);
	
}

void UInventorySlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePosition();
}

void UInventorySlot::ShowHoveredInfoWidget(int32 indexNumber)
{
	if(TabWidget->TabHoveredInfoWidget&&TabWidget->inventoryArray.IsValidIndex(indexNumber-1))
	{
		bHovered=true;
		TabHoveredInfoWidget->WidgetSwitcher_Info->SetActiveWidgetIndex(TabWidget->switcherIndexMap[TabWidget->inventoryArray[indexNumber-1]]);
		TabHoveredInfoWidget->AddToViewport();
	}
}

void UInventorySlot::HideHoveredInfoWidget()
{
	if(TabHoveredInfoWidget->IsInViewport())
	{
		bHovered=false;
		TabHoveredInfoWidget->RemoveFromParent();
	}
}

void UInventorySlot::UpdatePosition()
{
	if(bHovered&&pc)
	{
		float X;
		float Y;
		pc->GetMousePosition(X, Y);
		auto mousePos = UKismetMathLibrary::MakeVector2D(X, Y);
		TabHoveredInfoWidget->SetPositionInViewport(mousePos);
	}
}

void UInventorySlot::SetThumbnail()
{
	if(DragThumbnail&&TabWidget->inventoryArray.IsValidIndex(1))
	{
		DragThumbnail->WidgetSwitcher_thumbnail->SetActiveWidgetIndex(TabWidget->switcherIndexMap[TabWidget->inventoryArray[1]]);
	}
}
