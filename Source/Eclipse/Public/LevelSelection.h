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
	class UButton* SelectLevel1Yes;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* SelectLevel1No;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* SelectLevel2Yes;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* SelectLevel2No;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* IsolatedShipButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* ExitButton;
	
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* LevelSelectionStartAnim;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* LevelSelectionEndAnim;
	
	UPROPERTY()
	class AEclipsePlayerController* pc;

	UPROPERTY()
	class APlayerCharacter* player;

	UPROPERTY()
	class UEclipseGameInstance* gi;

	UPROPERTY()
	bool quitBool = false;
	
	UFUNCTION(BlueprintCallable)
	void Level1Y();

	UFUNCTION(BlueprintCallable)
	void Level1N();

	UFUNCTION(BlueprintCallable)
	void Level2Y();

	UFUNCTION(BlueprintCallable)
	void Level2N();

	UFUNCTION(BlueprintCallable)
	void SelectExitGame();

	UFUNCTION(BlueprintCallable)
	void OpenMoveIsolatedShipSelection();

	UPROPERTY(EditAnywhere, Category=Sound)
	class USoundBase* QuitSound;

	UPROPERTY(EditAnywhere, Category=Sound)
	class USoundBase* CloseSound;

	UPROPERTY(EditAnywhere, Category=Sound)
	class USoundBase* AskSound;

private:
};
