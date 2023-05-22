// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SuperDimension : ModuleRules
{
	public SuperDimension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { "PhysicsCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "UMG" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Niagara" });
        PrivateDependencyModuleNames.AddRange(new string[] { "AIModule" });
        PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput" });
        PrivateDependencyModuleNames.AddRange(new string[] { "AssetRegistry" });
        PrivateDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks" });
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemSteam" });
        PrivateDependencyModuleNames.AddRange(new string[] { "GameplayInventories" });

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd" });
        }
    }
}
