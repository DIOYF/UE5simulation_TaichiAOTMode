// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class TaichiProjectFF : ModuleRules
{
	private string project_root_path
	{
		get { return Path.Combine(ModuleDirectory, "../.."); }
	}

	public TaichiProjectFF(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		string my_taichi_c_api_include = "TaichiCAPI/include";
		string my_taichi_c_api_lib = "TaichiCAPI/lib";
		string my_taichi_c_api_dll = "TaichiCAPI/bin";

		PublicIncludePaths.Add(Path.Combine(project_root_path, my_taichi_c_api_include));
		PublicAdditionalLibraries.Add(Path.Combine(project_root_path, my_taichi_c_api_lib, "taichi_c_api.lib"));
		RuntimeDependencies.Add(Path.Combine(project_root_path, my_taichi_c_api_dll,"taichi_c_api.dll"));

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
