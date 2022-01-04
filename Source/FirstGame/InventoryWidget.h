// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyBaseWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UInventoryWidget : public UMyBaseWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* closeBtn;

	UPROPERTY(meta=(BindWidget))
	class UListView* itemListView;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void CloseButtonClick();

	void UpdateInventory();

	virtual void Open() override;
	virtual void Close() override;
};
