// Fill out your copyright notice in the Description page of Project Settings.

#define LOCTEXT_NAMESPACE "FarmGame"

FText progressText = LOCTEXT("key0", "{0}/{1}");

#undef LOCTEXT_NAMESPACE

#include "hpBarWidget.h"
#include <Components/TextBlock.h>
#include <Components/ProgressBar.h>

bool UhpBarWidget::Initialize()
{
	Super::Initialize();

	text->SetText(FText::Format(progressText, 80, 100));
	hpBar->SetPercent(.8);

	return true;
}
