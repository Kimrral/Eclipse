// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_Menu;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* Btn_ReturnToHideout;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* btn_Settings;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* btn_QuitGame;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* MenuStart;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* MenuEnd;
	
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* HideOutSelectionStart;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* QuitSelectionStart;	

	UPROPERTY()
	class AEclipsePlayerController* pc;

	UPROPERTY()
	class APlayerCharacter* player;

	UPROPERTY()
	class UEclipseGameInstance* gi;

	UPROPERTY()
	bool quitBool = false;

	UPROPERTY(EditAnywhere, Category=Sound)
	class USoundBase* QuitSound;

	UPROPERTY(EditAnywhere, Category=Sound)
	class USoundBase* CloseSound;

	UPROPERTY(EditAnywhere, Category=Sound)
	class USoundBase* AskSound;

	UFUNCTION(BlueprintCallable)
	void ReturnToHideoutFunc();

	UFUNCTION(BlueprintCallable)
	void ReturnToHideoutYesFunc();

	UFUNCTION(BlueprintCallable)
	void ReturnToHideoutNoFunc();
	
	UFUNCTION(BlueprintCallable)
	void SelectExitGameFunc();

	UFUNCTION(BlueprintCallable)
	void SelectExitGameYesFunc();

	UFUNCTION(BlueprintCallable)
	void SelectExitGameNoFunc();

	UFUNCTION()
	void CloseWidgetFunc();
	
	
};
