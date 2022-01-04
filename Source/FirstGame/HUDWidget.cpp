// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "FirstGameGameModeBase.h"

void UHUDWidget::NativeConstruct()
{
	if (inventoryBtn != nullptr)
		inventoryBtn->OnClicked.AddDynamic(this, &UHUDWidget::InventoryButtonClick);
}

void UHUDWidget::InventoryButtonClick()
{
	AFirstGameGameModeBase* gameMode = GetWorld()->GetAuthGameMode<AFirstGameGameModeBase>();
	gameMode->OpenWidget(WIDGET_TYPE::INVENTORY);
}



