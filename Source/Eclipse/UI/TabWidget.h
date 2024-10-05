// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Eclipse/GameData/PlayerInventoryStruct.h"
#include "TabWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UTabWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class APlayerCharacter* player;

	UFUNCTION(BlueprintCallable)
	void SetTabWidget();

	UFUNCTION(BlueprintCallable)
	void UpdateAmmunition();

	UFUNCTION(BlueprintCallable)
	void UpdateHealthPoint();
	
	UFUNCTION()
	static TArray<FPlayerInventoryStruct> GetInventoryData();
	
	UFUNCTION()
	static TArray<int32> GetInventoryStacks();

	UPROPERTY()
	class AEclipsePlayerController* pc;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UTabHoveredInfoWidget* TabHoveredInfoWidget;

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

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* CurHPText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* MaxHPText;

};
