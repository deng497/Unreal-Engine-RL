// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Test_Test : ModuleRules
{
	public Test_Test(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ChaosVehicles", "PhysicsCore", "LearningAgents", "LearningAgentsTraining"});
	}
}
