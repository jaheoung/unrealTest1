// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyBaseWidget.h"
#include "NpcIconWidget.h"
#include "MinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UMinimapWidget : public UMyBaseWidget
{
	GENERATED_BODY()

	UMinimapWidget();
	
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* clippingPanel;
	UPROPERTY(meta=(BindWidget))
	class UImage* mapImage;
	UPROPERTY(meta=(BindWidget))
	class UImage* myUnitIcon;
	
	TSubclassOf<UNpcIconWidget> npcIconWidgetClass;

	TMap<uint32_t, UNpcIconWidget*> npcIconMap;
	
	FVector2D mapPos;
	bool isInit = false;
	float imageW = 0;
	float imageH = 0;
	float imgRate = 0;
	float halfClippingW = 0;
	float halfClippingH = 0;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
