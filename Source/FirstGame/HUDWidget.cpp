// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "FirstGameGameModeBase.h"

void UHUDWidget::NativeConstruct()
{
	inventoryBtn->OnClicked.AddDynamic(this, &UHUDWidget::InventoryButtonClick);
	optionBtn->OnClicked.AddDynamic(this, &UHUDWidget::OptionButtonClick);
}

void UHUDWidget::InventoryButtonClick()
{
	AFirstGameGameModeBase* gameMode = GetWorld()->GetAuthGameMode<AFirstGameGameModeBase>();
	gameMode->OpenWidget(WIDGET_TYPE::INVENTORY);
}

void UHUDWidget::OptionButtonClick()
{
	AFirstGameGameModeBase* gameMode = GetWorld()->GetAuthGameMode<AFirstGameGameModeBase>();
	gameMode->OpenWidget(WIDGET_TYPE::OPTION);
}




