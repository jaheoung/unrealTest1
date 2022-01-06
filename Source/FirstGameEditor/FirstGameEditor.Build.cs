using UnrealBuildTool;

public class FirstGameEditor : ModuleRules
{
	public FirstGameEditor(ReadOnlyTargetRules Target) : base (Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "RHI", "RenderCore",
			/*"ShaderCore",*/ "MainFrame", "AssetTools", "AppFramework", "PropertyEditor"
		});
		
		PublicDependencyModuleNames.Add("FirstGame");
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd", "Slate", "SlateCore", "EditorStyle", "GraphEditor",
			"BlueprintGraph"
		});
	}
}
