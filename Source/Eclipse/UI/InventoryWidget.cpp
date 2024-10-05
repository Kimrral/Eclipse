// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/UI/InventoryWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Eclipse/Player/EclipsePlayerController.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InventoryController = Cast<AEclipsePlayerController>(GetWorld()->GetFirstPlayerController());
	OwningPlayer = Cast<APlayerCharacter>(GetOwningLocalPlayer());
	
}

bool UInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (InOperation && InOperation->Payload)
	{
		if (Cast<UUserWidget>(InOperation->Payload))
		{
			// 인덱스 기반으로 AInventoryController의 인벤토리 조작 함수 호출 (Role : Controller)
			if (IsValid(InventoryController))
			{
				InventoryController->HandleDragAndDrop(DraggedIndex, DroppedIndex);
			}
			return true;
		}
	}
	return false;	
}

void UInventoryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// 드래그 앤 드롭 오퍼레이션 생성
	UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>();
	DragDropOp->Payload = this;  // 드래그할 데이터를 설정
	DragDropOp->DefaultDragVisual = this;  // 드래그 시 보여줄 비주얼 설정

	OutOperation = DragDropOp;
}




