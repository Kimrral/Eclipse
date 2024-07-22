// Fill out your copyright notice in the Description page of Project Settings.


#include "Eclipse/UI/ExtractionCountdown.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"

void UExtractionCountdown::NativeConstruct()
{
	Super::NativeConstruct();

	// CountdownConstruct 애니메이션을 재생합니다.
	PlayAnimation(CountdownConstruct);
    
	// bExtraction 변수를 false로 초기화합니다.
	bExtraction = false;
}

void UExtractionCountdown::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// SetCountdownTimer 함수를 호출하여 카운트다운 타이머를 설정합니다.
	SetCountdownTimer();
}

void UExtractionCountdown::SetCountdownTimer()
{
	// Countdown 값이 0.0f인지 확인합니다.
	if (Countdown == 0.0f)
	{
		// bExtraction 변수가 false인 경우
		if (!bExtraction)
		{
			// ExtractionSuccessDele 델리게이트를 브로드캐스트합니다.
			ExtractionSuccessDele.Broadcast();
            
			// bExtraction 변수를 true로 설정합니다.
			bExtraction = true;
		}
	}

	// Countdown 값을 현재 프레임의 델타 시간만큼 감소시킵니다.
	// FMath::Clamp 함수를 사용하여 Countdown 값을 0.f에서 10.f 사이로 제한합니다.
	Countdown = FMath::Clamp(Countdown -= GetWorld()->GetDeltaSeconds(), 0.f, 10.f);

	// Countdown 값을 문자열로 변환합니다.
	const FString CountdownString = UKismetStringLibrary::TimeSecondsToString(Countdown);
    
	// 변환된 문자열을 CountdownValue 텍스트 위젯에 설정합니다.
	CountdownValue->SetText(FText::FromString(CountdownString));
}
