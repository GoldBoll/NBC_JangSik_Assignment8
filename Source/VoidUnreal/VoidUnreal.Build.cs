using UnrealBuildTool;

public class VoidUnreal : ModuleRules
{
	public VoidUnreal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"AIModule",
				"GameplayTasks",
				"NavigationSystem",
				"UMG",
				"EnhancedInput"
			});
	}
}
