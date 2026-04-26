using UnrealBuildTool;

public class DQSEditor : ModuleRules
{
	public DQSEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"DQSUI",
				"Engine",
				"GameplayTags",
				"UnrealEd",
				"DQSRuntime"
			});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"ApplicationCore",
				"AssetRegistry",
				"AssetTools",
				"EditorFramework",
				"GraphEditor",
				"InputCore",
				"Kismet",
				"KismetCompiler",
				"KismetWidgets",
				"PropertyEditor",
				"Slate",
				"SlateCore",
				"ToolMenus",
				"Projects",
				"UMG",
				"UMGEditor"
			});
	}
}
