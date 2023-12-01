// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Hypergraph : ModuleRules
{
	public Hypergraph(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicIncludePaths.AddRange(new string[]{ });
		PrivateIncludePaths.AddRange(new string[]{ });
		PublicIncludePathModuleNames.AddRange(new string[]{ });
		PrivateIncludePathModuleNames.AddRange(new string[]{ });
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"EnhancedInput",
			"OnlineSubsystem",
			"OnlineSubsystemSteam",
			"UMG",
			"SlateCore",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
		});
	}
}
