// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlot.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UTabWidget* TabWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTabWidget> TabWidgetFactory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UDragThumbnail* DragThumbnail;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UDragThumbnail> DragThumbnailFactory;

	UFUNCTION(BlueprintCallable)
	void ShowHoveredInfoWidget(int32 indexNumber);

	UFUNCTION(BlueprintCallable)
	void HideHoveredInfoWidget();

	UFUNCTION(BlueprintCallable)
	void UpdatePosition();

	UFUNCTION(BlueprintCallable)
	void SetThumbnail();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UTabHoveredInfoWidget* TabHoveredInfoWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTabHoveredInfoWidget> TabHoveredInfoWidgetFactory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bHovered;

	UPROPERTY()
	class AEclipsePlayerController* pc;

	// Inventory Image
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* InventoryImage_1;

};
