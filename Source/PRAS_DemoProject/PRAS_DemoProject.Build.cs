// Designed by Hitman's Store 2022

using UnrealBuildTool;

public class PRAS_DemoProject : ModuleRules
{
	public PRAS_DemoProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "RecoilAnimation" });
	}
}
