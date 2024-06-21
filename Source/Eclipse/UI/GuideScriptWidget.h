// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Eclipse/Player/EclipsePlayerController.h"
#include "GuideScriptWidget.generated.h"

/**
 * 
 */
UCLASS()
class ECLIPSE_API UGuideScriptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<APlayerCharacter> Player;
	
};
