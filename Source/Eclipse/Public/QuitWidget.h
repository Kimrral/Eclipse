// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuitWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UQuitWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* QuitYes;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* QuitNo;

	UPROPERTY()
	class AEclipsePlayerController* pc;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* QuitWidgetStartAnim;

	UPROPERTY()
	bool quitBool = false;
	
	UFUNCTION()
	void QuitSelectYes();

	UFUNCTION()
	void QuitSelectNo();

};
