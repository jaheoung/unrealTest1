// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryWidget.h"
#include "FirstGameGameModeBase.h"
#include "InventoryItemData.h"
#include "Components/Button.h"
#include "Components/ListView.h"

#define LOCTEXT_NAMESPACE "inventoryWidget"
FText itemNameFormat = LOCTEXT("itemNameFormat", "type:{0}, count:{1}");
#undef LOCTEXT_NAMESPACE

void UInventoryWidget::NativeConstruct()
{
	if (closeBtn != nullptr)
		closeBtn->OnClicked.AddDynamic(this, &UInventoryWidget::CloseButtonClick);

	
}

void UInventoryWidget::Open()
{
	UpdateInventory();
}

void UInventoryWidget::Close()
{
	
}

void UInventoryWidget::UpdateInventory()
{
	itemListView->ClearListItems();

	AFirstGameGameModeBase* gameMode = GetWorld()->GetAuthGameMode<AFirstGameGameModeBase>();
	
	if (gameMode == nullptr)
		return;

	for (const auto elem : gameMode->myItems)
	{
		if (elem == nullptr || elem.IsValid() == false)
			continue;
		
		UInventoryItemData* data = NewObject<UInventoryItemData>(); // 재사용할것.
		data->itemName = FText::Format(itemNameFormat, 1, elem->count);
		itemListView->AddItem(data);
	}
}


void UInventoryWidget::CloseButtonClick()
{
	AFirstGameGameModeBase* gameMode = GetWorld()->GetAuthGameMode<AFirstGameGameModeBase>();
	
	if (gameMode == nullptr)
		return;

	gameMode->CloseWidget(widgetType);
}

