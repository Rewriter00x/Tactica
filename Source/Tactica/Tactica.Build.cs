// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Tactica : ModuleRules
{
	public Tactica(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.Never;

		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
		PrivateDependencyModuleNames.AddRange(new[] { "UMG", "Slate" });
	}
}
