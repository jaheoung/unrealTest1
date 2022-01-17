// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyBaseWidget.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UMainWidget : public UMyBaseWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UMainCanvasPanel* mainCanvas;

	void AddChild(UWidget* widget);

	void TopWidget(UWidget* widget);
};
