#include "FirstGameEditor.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#include "LevelEditor.h"
#include "SlateBasics.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "MapTool.h"


IMPLEMENT_GAME_MODULE(FFirstGameEditor, FirstGameEditor)

void FFirstGameEditor::StartupModule()
{
	FMapTool::Register();

	TSharedPtr<FUICommandList> CommandList = MakeShareable(new FUICommandList());

	CommandList->MapAction(FMapTool::Get().myButton, FExecuteAction::CreateRaw(this,
		&FFirstGameEditor::MyButtonClick),
		FCanExecuteAction());

	ToolbarExtender = MakeShareable(new FExtender());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	Extension = ToolbarExtender->AddToolBarExtension("Compile",
		EExtensionHook::Before, CommandList,
		FToolBarExtensionDelegate::CreateRaw(this, &FFirstGameEditor::AddToolbarExtension));

	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

void FFirstGameEditor::ShutdownModule()
{
	ToolbarExtender->RemoveExtension(Extension.ToSharedRef());
	Extension.Reset();
	ToolbarExtender.Reset();
}
