// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StashWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UStashWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* ExitButton;

	virtual void NativeConstruct() override;

	UPROPERTY()
	class AEclipsePlayerController* pc;

	UPROPERTY()
	class APlayerCharacter* playerCPP;

	UFUNCTION()
	void CloseStashWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void StashConstruct();
	
};
