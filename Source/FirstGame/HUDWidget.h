// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyBaseWidget.h"
#include "Components/Button.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UHUDWidget : public UMyBaseWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* inventoryBtn;

	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void InventoryButtonClick();
};
