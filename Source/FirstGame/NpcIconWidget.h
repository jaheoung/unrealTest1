// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyBaseWidget.h"
#include "NpcIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UNpcIconWidget : public UMyBaseWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UImage* npcIcon;
};
