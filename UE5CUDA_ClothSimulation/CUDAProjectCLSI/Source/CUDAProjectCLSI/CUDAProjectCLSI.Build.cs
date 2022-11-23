// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;


public class CUDAProjectCLSI : ModuleRules
{
	private string project_root_path
	{
		get { return Path.Combine(ModuleDirectory, "../.."); }
	}

	public CUDAProjectCLSI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "RenderCore", "ProceduralMeshComponent" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		string my_cuda_lib_include = "CUDALib/include";
		string my_cuda_lib_lib = "CUDALib/lib";

		PublicIncludePaths.Add(Path.Combine(project_root_path, my_cuda_lib_include));
		PublicAdditionalLibraries.Add(Path.Combine(project_root_path, my_cuda_lib_lib, "CUDA_ClothSimulation.lib"));

		// Note to Include Your Own CUDA Path
		string cuda_path = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.7";
		string cuda_include = "include";
		string cuda_lib = "lib/x64";

		PublicIncludePaths.Add(Path.Combine(cuda_path, cuda_include));
		PublicAdditionalLibraries.Add(Path.Combine(cuda_path, cuda_lib, "cudart_static.lib"));

	}
}
