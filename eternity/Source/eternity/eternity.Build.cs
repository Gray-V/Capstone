// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class eternity : ModuleRules
{
	public eternity(ReadOnlyTargetRules Target) : base(Target)
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
			"eternity",
			"eternity/Variant_Platforming",
			"eternity/Variant_Platforming/Animation",
			"eternity/Variant_Combat",
			"eternity/Variant_Combat/AI",
			"eternity/Variant_Combat/Animation",
			"eternity/Variant_Combat/Gameplay",
			"eternity/Variant_Combat/Interfaces",
			"eternity/Variant_Combat/UI",
			"eternity/Variant_SideScrolling",
			"eternity/Variant_SideScrolling/AI",
			"eternity/Variant_SideScrolling/Gameplay",
			"eternity/Variant_SideScrolling/Interfaces",
			"eternity/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
