#pragma once
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

class FMapTool : public TCommands<FMapTool>
{
public:
	FMapTool() : TCommands<FMapTool>(
		FName(TEXT("MapTool")),
		FText::FromString("Map Tool"),
		NAME_None,
		FEditorStyle::GetStyleSetName())
	{

	};

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> myButton;
	TSharedPtr<FUICommandInfo> myMenuButton;
};
