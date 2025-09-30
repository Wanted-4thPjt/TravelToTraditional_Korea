// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE_TTTK : ModuleRules
{
	public UE_TTTK(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"UE_TTTK",
			"UE_TTTK/Variant_Platforming",
			"UE_TTTK/Variant_Platforming/Animation",
			"UE_TTTK/Variant_Combat",
			"UE_TTTK/Variant_Combat/AI",
			"UE_TTTK/Variant_Combat/Animation",
			"UE_TTTK/Variant_Combat/Gameplay",
			"UE_TTTK/Variant_Combat/Interfaces",
			"UE_TTTK/Variant_Combat/UI",
			"UE_TTTK/Variant_SideScrolling",
			"UE_TTTK/Variant_SideScrolling/AI",
			"UE_TTTK/Variant_SideScrolling/Gameplay",
			"UE_TTTK/Variant_SideScrolling/Interfaces",
			"UE_TTTK/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
