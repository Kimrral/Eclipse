// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelSelection.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API ULevelSelection : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_Level;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* Level1Yes;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* Level1No;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* Level2Yes;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* Level2No;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* IsolatedShipButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* ExitButton;
	
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* LevelSelectionStartAnim;
	
	UPROPERTY()
	class AEclipsePlayerController* pc;

	UPROPERTY()
	class APlayerCharacter* player;

	UPROPERTY()
	bool quitBool = false;
	
	UFUNCTION()
	void Level1Y();

	UFUNCTION()
	void Level1N();

	UFUNCTION()
	void Level2Y();

	UFUNCTION()
	void Level2N();

	UFUNCTION()
	void Exit();

	UFUNCTION()
	void OpenMoveIsolatedShipSelection();

private:
};
