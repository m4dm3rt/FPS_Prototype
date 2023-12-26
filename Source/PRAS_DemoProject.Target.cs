// Designed by Hitman's Store 2022

using UnrealBuildTool;
using System.Collections.Generic;

public class PRAS_DemoProjectTarget : TargetRules
{
	public PRAS_DemoProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("PRAS_DemoProject");
	}
}
