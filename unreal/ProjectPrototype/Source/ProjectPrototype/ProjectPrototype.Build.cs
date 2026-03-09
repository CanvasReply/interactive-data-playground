using UnrealBuildTool;

public class ProjectPrototype : ModuleRules
{
    public ProjectPrototype(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HTTP"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Json",
            "JsonUtilities"
        });

        // PrivateDependencyModuleNames.AddRange(new[] { "Slate", "SlateCore" });
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
    }
}