// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemData.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UInventoryItemData : public UObject
{
	GENERATED_BODY()

public:
	FText itemName;
	UTexture2D* itemTex;
};
