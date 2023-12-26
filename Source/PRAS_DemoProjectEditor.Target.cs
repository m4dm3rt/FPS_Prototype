// Designed by Hitman's Store 2022

using UnrealBuildTool;
using System.Collections.Generic;

public class PRAS_DemoProjectEditorTarget : TargetRules
{
	public PRAS_DemoProjectEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("PRAS_DemoProject");
	}
}
