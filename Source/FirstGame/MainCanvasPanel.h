// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "MainCanvasPanel.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UMainCanvasPanel : public UCanvasPanel
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void TopWidget(UWidget* widget);
};
