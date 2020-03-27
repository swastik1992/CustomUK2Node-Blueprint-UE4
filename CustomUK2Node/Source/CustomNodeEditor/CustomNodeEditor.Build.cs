// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class CustomNodeEditor : ModuleRules
{
	public CustomNodeEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {
			"BlueprintGraph",
			"UnrealEd",
			"CustomUK2Node",
			"Slate",
			"SlateCore",
			"EditorStyle",
			"GraphEditor",
			"KismetCompiler",
			"ToolMenus"

		});
	}
}
