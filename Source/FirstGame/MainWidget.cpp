// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "MainCanvasPanel.h"

void UMainWidget::AddChild(UWidget* widget)
{
	mainCanvas->AddChild(widget);
}

void UMainWidget::TopWidget(UWidget* widget)
{
	mainCanvas->TopWidget(widget);
}

