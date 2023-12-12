// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "TabWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UTabWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UFUNCTION()
	void SetInventoryImageArray();

	UFUNCTION()
	void SetInventoryTextureMap();

	UFUNCTION(BlueprintCallable)
	void ShowHoveredInfoWidget(int32 indexNumber);

	UFUNCTION()
	void SetSwitcherIndexMap();

	UFUNCTION(BlueprintCallable)
	void HideHoveredInfoWidget();

	UFUNCTION()
	void UpdatePosition();

	UPROPERTY()
	bool bHovered;

	UPROPERTY()
	class AEclipsePlayerController* pc;

	UPROPERTY()
	class UTabHoveredInfoWidget* TabHoveredInfoWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTabHoveredInfoWidget> TabHoveredInfoWidgetFactory;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FString> inventoryArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<int32> inventoryCountArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<UButton*> inventoryButtonArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<UImage*> inventoryImageArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, UTexture2D*> inventoryTextureMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, int32> switcherIndexMap;

	UFUNCTION()
	void SetInventoryArray(FString ActorString);
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* WeaponText1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* WeaponText2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* CurBulletText1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* CurBulletText2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* BulletText1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* BulletText2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* MaxBulletText1;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* MaxBulletText2;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* MaxBulletText3;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* MaxBulletText4;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage1_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage1_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage1_3;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage2_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage2_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurBulletImage2_3;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage1_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage1_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage1_3;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage2_1;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage2_2;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UImage* CurWeaponImage2_3;

	// Inventory Button
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Inventory_1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Inventory_2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Inventory_3;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Inventory_4;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Inventory_5;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Inventory_6;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Inventory_7;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Inventory_8;

	// Inventory Image
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* InventoryImage_1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* InventoryImage_2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* InventoryImage_3;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* InventoryImage_4;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* InventoryImage_5;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* InventoryImage_6;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* InventoryImage_7;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* InventoryImage_8;

	//Inventory Texture
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* rifleMagTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* sniperMagTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* PistolMagTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* M249MagTexture;

	UPROPERTY()
	bool bIndividual;
	
	
};
