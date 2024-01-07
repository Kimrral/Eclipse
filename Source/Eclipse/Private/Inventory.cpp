// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

#include "Components/Button.h"

void UInventory::ButtonConstruct()
{
	UseButton->OnClicked.AddDynamic(this, &UInventory::UseButtonClicked);
	CloseButton->OnClicked.AddDynamic(this, &UInventory::CloseButtonClicked);
}

