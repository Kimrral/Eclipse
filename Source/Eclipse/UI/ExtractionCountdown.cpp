// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/UI/ExtractionCountdown.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"

void UExtractionCountdown::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimation(CountdownConstruct);
}

void UExtractionCountdown::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	SetCountdownTimer();
}

void UExtractionCountdown::SetCountdownTimer()
{
	if (Countdown == 0.0f)
	{
		if (bExtraction == true)
		{
			return;
		}
		ExtractionSuccessDele.Broadcast();
		bExtraction = true;
	}
	Countdown = FMath::Clamp(Countdown -= GetWorld()->GetDeltaSeconds(), 0.f, 10.f);
	const FString CountdownString = UKismetStringLibrary::TimeSecondsToString(Countdown);
	CountdownValue->SetText(FText::FromString(CountdownString));
}
