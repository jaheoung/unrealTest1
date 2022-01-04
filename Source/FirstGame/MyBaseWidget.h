// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API UMyBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	enum class WIDGET_TYPE widgetType;

	virtual void Open();
	virtual void Close();
	
	// 인자로 넘겨준 컴포넌트를 유저 화면을 바라보게 한다.
	static void LookAtWidgetComponent(class UWorld* world, class UWidgetComponent* comp);
};
