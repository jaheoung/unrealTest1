// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyBaseWidget.h"
#include "Components/Button.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UInteractionWidget : public UMyBaseWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta=(BindWidget))
	class UButton* interactionBtn;

	virtual void NativeConstruct() override;
};
