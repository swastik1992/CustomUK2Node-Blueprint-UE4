// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class CustomUK2NodeEditorTarget : TargetRules
{
	public CustomUK2NodeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "CustomUK2Node", "CustomNodeEditor" } );
	}
}
