// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryWidget.h"
#include "FirstGameGameModeBase.h"
#include "InventoryItemData.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "ServerActor.h"

#define LOCTEXT_NAMESPACE "inventoryWidget"
FText itemNameFormat = LOCTEXT("itemNameFormat", "count:{0}");
#undef LOCTEXT_NAMESPACE

UInventoryWidget::UInventoryWidget()
{
	TMap<ITEM_TYPE, TCHAR*> pathMap;
	pathMap.Add(ITEM_TYPE::MEAT, TEXT("Texture2D'/Game/MyResource/Images/meat.meat'"));
	pathMap.Add(ITEM_TYPE::GOLD, TEXT("Texture2D'/Game/MyResource/Images/gold.gold'"));

	for (auto elem : pathMap)
	{
		auto loadAsset = ConstructorHelpers::FObjectFinder<UTexture2D>(elem.Value);
		if (loadAsset.Succeeded())
		{
			texSourceMap.Add(elem.Key, loadAsset.Object);
		}
	}
}


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
		data->itemName = FText::Format(itemNameFormat, elem->count);
		
		UTexture2D** getTex = texSourceMap.Find(elem->type);
		if (getTex != nullptr && (*getTex) != nullptr)
			data->itemTex = *getTex;
		
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

