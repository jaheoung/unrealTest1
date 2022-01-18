// Fill out your copyright notice in the Description page of Project Settings.


#include "OptionWidget.h"

#include "FirstGameGameModeBase.h"
#include "ImageLoader.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UOptionWidget::NativeConstruct()
{
	closeBtn->OnClicked.AddDynamic(this, &UOptionWidget::CloseButtonClick);

	UImageLoader* loader = NewObject<UImageLoader>();
	
	//testImage->SetBrushFromTexture(loader->ASyncLoad());
	
	loader->asyncGroupLoadCallback.BindLambda([this](TArray<UTexture2D*> texs)->void
	{
		int i = 0;
		for (auto elem:texs)
		{
			if (i == 0)
				testImage->SetBrushFromTexture(elem);
			else
				testImage2->SetBrushFromTexture(elem);
			++i;
		}
	});

	loader->ASyncGroupLoad();

	// int i = 0;
	// for (auto elem : loader->SyncGroupLoad())
	// {
	// 	if (i == 0)
	// 		testImage->SetBrushFromTexture(elem);
	// 	else
	// 		testImage2->SetBrushFromTexture(elem);
	// 	
	// 	++i;
	// }
}


void UOptionWidget::CloseButtonClick()
{
	AFirstGameGameModeBase* gameMode = GetWorld()->GetAuthGameMode<AFirstGameGameModeBase>();
	
	if (gameMode == nullptr)
		return;

	gameMode->CloseWidget(widgetType);
}
