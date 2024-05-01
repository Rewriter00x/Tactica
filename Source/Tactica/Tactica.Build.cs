// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Tactica : ModuleRules
{
	public Tactica(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
		PrivateDependencyModuleNames.AddRange(new[] { "UMG", "Slate" });
	}
}
