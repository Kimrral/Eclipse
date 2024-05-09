
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Styling/SlateColor.h"
#include "PlayerInventoryStruct.generated.h"

USTRUCT(BlueprintType)
struct FPlayerInventoryStruct : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	FPlayerInventoryStruct(): Thumbnail(nullptr), Price(0), Stat(0){}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	const UTexture2D* Thumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	FString ShowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	FString ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	FString ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	FSlateColor TypeColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	TSubclassOf<AActor> ActorReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	int32 Price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	float Stat;


};
