using UnrealBuildTool;

public class DQSUI : ModuleRules
{
	public DQSUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"DeveloperSettings",
				"Engine",
				"UMG",
				"DQSRuntime"
			});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Slate",
				"SlateCore"
			});
	}
}
