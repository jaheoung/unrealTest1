// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventoryItemWidget.generated.h"


/**
 * 
 */
UCLASS()
class FIRSTGAME_API UInventoryItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* itemText;
	
protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
