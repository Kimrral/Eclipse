// Fill out your copyright notice in the Description page of Project Settings.


#include "TabWidget.h"

#include "DragThumbnail.h"
#include "EclipsePlayerController.h"
#include "TabHoveredInfoWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


void UTabWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetInventoryImageArray();

	SetInventoryTextureMap();

	SetSwitcherIndexMap();

	inventoryCountArray.SetNum(29);

	pc=Cast<AEclipsePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	TabHoveredInfoWidget=CreateWidget<UTabHoveredInfoWidget>(GetWorld(), TabHoveredInfoWidgetFactory);

	DragThumbnail=CreateWidget<UDragThumbnail>(GetWorld(), DragThumbnailFactory);
	

}

void UTabWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdatePosition();
}

void UTabWidget::SetInventoryImageArray()
{
	inventoryImageArray.Add(InventoryImage_1);
	inventoryImageArray.Add(InventoryImage_2);
	inventoryImageArray.Add(InventoryImage_3);
	inventoryImageArray.Add(InventoryImage_4);

}

void UTabWidget::SetInventoryTextureMap()
{
	inventoryTextureMap.Add(FString("RifleMagActor"), rifleMagTexture);
	inventoryTextureMap.Add(FString("SniperMagActor"), sniperMagTexture);	
	inventoryTextureMap.Add(FString("PistolMagActor"), PistolMagTexture);	
	inventoryTextureMap.Add(FString("M249MagActor"), M249MagTexture);	
	
}

void UTabWidget::SetSwitcherIndexMap()
{
	switcherIndexMap.Add(FString("RifleMagActor"), 5);
	switcherIndexMap.Add(FString("SniperMagActor"), 6);	
	switcherIndexMap.Add(FString("PistolMagActor"), 7);	
	switcherIndexMap.Add(FString("M249MagActor"), 8);	
}

void UTabWidget::ShowHoveredInfoWidget(int32 indexNumber)
{
	if(TabHoveredInfoWidget&&inventoryArray.IsValidIndex(indexNumber-1))
	{
		bHovered=true;
		TabHoveredInfoWidget->WidgetSwitcher_Info->SetActiveWidgetIndex(switcherIndexMap[inventoryArray[indexNumber-1]]);
		TabHoveredInfoWidget->AddToViewport();
	}
}

void UTabWidget::HideHoveredInfoWidget()
{
	if(TabHoveredInfoWidget)
	{
		bHovered=false;
		TabHoveredInfoWidget->RemoveFromParent();
	}
}

void UTabWidget::ShowDragThumbnail(int32 indexNumber)
{
	if(DragThumbnail&&inventoryArray.IsValidIndex(indexNumber-1))
	{
		bShowThumbnail=true;
		DragThumbnail->WidgetSwitcher_thumbnail->SetActiveWidgetIndex(switcherIndexMap[inventoryArray[indexNumber-1]]);
		DragThumbnail->AddToViewport();
	}
}

void UTabWidget::HideDragThumbnail()
{
	if(DragThumbnail->IsInViewport())
	{
		bShowThumbnail=false;
		DragThumbnail->RemoveFromParent();
	}
}

void UTabWidget::UpdatePosition()
{
	if(bHovered&&pc)
	{
		float X;
		float Y;
		pc->GetMousePosition(X, Y);
		auto mousePos = UKismetMathLibrary::MakeVector2D(X, Y);
		TabHoveredInfoWidget->SetPositionInViewport(mousePos);
	}
	if(bShowThumbnail&&pc)
	{
		float X;
		float Y;
		pc->GetMousePosition(X, Y);
		auto mousePosT = UKismetMathLibrary::MakeVector2D(X, Y);
		DragThumbnail->SetPositionInViewport(mousePosT);
	}
}

void UTabWidget::SetInventoryArray(FString ActorString)
{
	// 인벤토리 배열 크기가 1보다 클때
	if(inventoryArray.Num()>=1)
	{
		// 인벤토리 액터 배열 순회
		for(int i=0; i<inventoryArray.Num(); i++)
		{
			// 액터 배열 중에 중복되는 액터가 있다면
			if(inventoryArray[i]==ActorString)
			{
				inventoryCountArray[i]++;
				UE_LOG(LogTemp, Warning, TEXT("SetInventorytrue"))
				return;
			}
			// 중복되는 액터가 없다면
			else
			{
				bIndividual=true;
				UE_LOG(LogTemp, Warning, TEXT("SetInventoryfalse"))
			}
		}
	}
	if(bIndividual||inventoryArray.Num()<1)
	{
		// 인벤토리 액터 배열에 추가
		int32 arrayIndex = inventoryArray.Num();
		inventoryArray.Emplace(ActorString);		
		// 현재 인벤토리 칸의 액터 개수 +1
		inventoryCountArray[arrayIndex]+=1;
		// 인벤토리 이미지 설정
		inventoryImageArray[arrayIndex]->SetBrushFromTexture(inventoryTextureMap[ActorString], true);
		inventoryImageArray[arrayIndex]->SetVisibility(ESlateVisibility::HitTestInvisible);
		bIndividual=false;		
	 }	
}
