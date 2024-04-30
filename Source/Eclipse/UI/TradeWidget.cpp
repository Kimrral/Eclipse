// Fill out your copyright notice in the Description page of Project Settings.


#include "TradeWidget.h"

#include "Components/TextBlock.h"

void UTradeWidget::UpdateRoubleText(const int32 CurrentRoubles) const
{
	CurrentRouble->SetText(FText::AsNumber(CurrentRoubles));
}
