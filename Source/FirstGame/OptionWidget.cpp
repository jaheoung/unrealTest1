// Fill out your copyright notice in the Description page of Project Settings.


#include "OptionWidget.h"

#include "FirstGameGameModeBase.h"
#include "Components/Button.h"

void UOptionWidget::NativeConstruct()
{
	closeBtn->OnClicked.AddDynamic(this, &UOptionWidget::CloseButtonClick);
}


void UOptionWidget::CloseButtonClick()
{
	AFirstGameGameModeBase* gameMode = GetWorld()->GetAuthGameMode<AFirstGameGameModeBase>();
	
	if (gameMode == nullptr)
		return;

	gameMode->CloseWidget(widgetType);
}
