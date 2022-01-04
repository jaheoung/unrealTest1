// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemWidget.h"

#include "InventoryItemData.h"
#include "Components/TextBlock.h"

void UInventoryItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UInventoryItemData* data = Cast<UInventoryItemData>(ListItemObject);

	if (data == nullptr)
		return;

	itemText->SetText(data->itemName);
}
