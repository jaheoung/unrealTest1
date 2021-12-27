// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyBaseWidget.h"
#include "hpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UhpBarWidget : public UMyBaseWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* hpBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* text;

	void SetHp(const float& hp, const float& maxHp);

	virtual bool Initialize() override;
};
