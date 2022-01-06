#include "MapTool.h" // 무조건 맨위에 인클루드 되어야 함!!
#include "Framework/Commands/Commands.h"
#include "FirstGAmeEditor.h"


void FMapTool::RegisterCommands()
{
#define LOCTEXT_NAMESPACE ""

	UI_COMMAND(myButton, "MapTool", "First Game Map Tool", EUserInterfaceActionType::Button, FInputGesture());

	UI_COMMAND(myMenuButton, "MapTool", "First Game Map Tool", EUserInterfaceActionType::Button, FInputGesture());

#undef LOCTEXT_NAMESPACE
}
