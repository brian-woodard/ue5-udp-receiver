// Copyright 2020-2021 CesiumGS, Inc. and Contributors

using UnrealBuildTool;

public class CesiumForUnrealSamples : ModuleRules
{
	public CesiumForUnrealSamples(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Sockets", "Networking" });

		PrivateDependencyModuleNames.AddRange(new string[] { "CesiumRuntime" });

        PrivateIncludePaths.AddRange(new string[] { "CesiumRuntime/Public",
                                                    "CesiumRuntime/Private" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
