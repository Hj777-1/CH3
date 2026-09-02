using UnrealBuildTool;

public class CH_3 : ModuleRules
{
    public CH_3(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "EnhancedInput",
        "UMG" 
			}
        );

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}