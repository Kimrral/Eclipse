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
	class UButton* SelectQuitYes;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* SelectQuitNo;

	UPROPERTY()
	class AEclipsePlayerController* pc;

	UPROPERTY()
	class UEclipseGameInstance* gi;

	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* QuitWidgetStartAnim;

	UPROPERTY(EditAnywhere, Category=Sound)
	class USoundBase* CloseSound;

	UFUNCTION(BlueprintCallable)
	void QuitSelectYes();

	UFUNCTION(BlueprintCallable)
	void QuitSelectNo();
};
