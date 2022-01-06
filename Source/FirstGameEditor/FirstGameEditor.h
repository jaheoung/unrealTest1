#pragma once

#include "Engine.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"

#include "MapTool.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"


class FFirstGameEditor : public IModuleInterface
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FExtender> ToolbarExtender;
	TSharedPtr<const FExtensionBase> Extension;

	void MyButtonClick()
	{
		// 이 부분이 윈도우를 세팅하는 부분이다.
		TSharedRef<SWindow> MapToolWindow = SNew(SWindow)
			.Title(FText::FromString(TEXT("Map Tool")))
			.ClientSize(FVector2D(800, 400))
			.SupportsMaximize(false)
			.SupportsMinimize(false)
			[
				SNew(SVerticalBox) + SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock).Text(FText::FromString("hello map tool~"))
				]
			];

		IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));

		if (MainFrameModule.GetParentWindow().IsValid())
		{
			FSlateApplication::Get().AddWindowAsNativeChild(MapToolWindow, MainFrameModule.GetParentWindow().ToSharedRef());
		}
		else
		{
			FSlateApplication::Get().AddWindow(MapToolWindow);
		}
	};

	void AddToolbarExtension(FToolBarBuilder& builder)
	{
		FSlateIcon IconBrush = FSlateIcon(FEditorStyle::GetStyleSetName(),
			"LevelEditor.ViewOptions",
			"LevelEditor.ViewOptions.Small");

		builder.AddToolBarButton(FMapTool::Get().myButton, NAME_None, FText::FromString("My Button"),
			FText::FromString("Click me to display a message"), IconBrush, NAME_None);
	};

};
