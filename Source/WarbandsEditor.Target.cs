using UnrealBuildTool;
public class WarbandsEditorTarget : TargetRules
{
    public WarbandsEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("Warbands");
    }
}
