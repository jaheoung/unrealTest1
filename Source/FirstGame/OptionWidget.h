// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyBaseWidget.h"
#include "OptionWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UOptionWidget : public UMyBaseWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* closeBtn;
	UPROPERTY(meta=(BindWidget))
	class UImage* testImage;
	UPROPERTY(meta=(BindWidget))
	class UImage* testImage2;

	UFUNCTION()
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void CloseButtonClick();
};
