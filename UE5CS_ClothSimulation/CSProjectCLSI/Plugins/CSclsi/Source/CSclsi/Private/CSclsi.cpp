// Copyright Epic Games, Inc. All Rights Reserved.

#include "CSclsi.h"
#include "Interfaces/IPluginManager.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"

#include "RenderGraphBuilder.h"
#include"RHI.h"

#define LOCTEXT_NAMESPACE "FCSclsiModule"

void FCSclsiModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Plugins/CSclsi/Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/CSclsi"), PluginShaderDir);
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FCSclsiModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}


void FCSclsiModule::BeginRendering()
{
	if (OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}

	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		OnPostResolvedSceneColorHandle = RendererModule->GetResolvedSceneColorCallbacks().AddRaw(this, &FCSclsiModule::RunComputeShader_RenderThread);
	}

	const size_t size = sizeof(FVector3f);


	TResourceArray<FVector3f> velocityArray;
	TResourceArray<FVector3f> positionArray;
	
	for (int i = 0; i < arr_size_x; i++)
	{
		for (int j = 0; j < arr_size_y; j++)
		{
			float pos_x = i * 1.0 / arr_size_x  - 0.5 +  FMath::RandRange(-0.05f, 0.05f)*0.1;
			//float pos_x = i * 1.0 / arr_size_x - 0.5;
			float pos_y = j * 1.0 / arr_size_y  - 0.5 +  FMath::RandRange(-0.05f, 0.05f)*0.1;
			//float pos_y = j * 1.0 / arr_size_y - 0.5;
			float pos_z = 0.6;
			Positions.Add(FVector3f(pos_x, pos_y, pos_z));
			positionArray.Add(FVector3f(pos_x, pos_y, pos_z));
			velocityArray.Add(FVector3f(0., 0., 0.));
			
		}
	}

	FRHIResourceCreateInfo createInfo(TEXT("Position_Array"));

	createInfo.ResourceArray = &velocityArray;
	myShaderUsageConfigParameters.Vel = RHICreateStructuredBuffer(size, size * arr_size_x * arr_size_y, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
	myShaderUsageConfigParameters.VelBufferUAV = RHICreateUnorderedAccessView(myShaderUsageConfigParameters.Vel, false, false);

	createInfo.ResourceArray = &positionArray;
	myShaderUsageConfigParameters.Pos = RHICreateStructuredBuffer(size, size * arr_size_x * arr_size_y, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
	myShaderUsageConfigParameters.PosBufferUAV = RHICreateUnorderedAccessView(myShaderUsageConfigParameters.Pos, false, false);
}




void FCSclsiModule::EndRendering()
{
	if (!OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}
	// Get the Renderer Module and remove our entry from the ResolvedSceneColorCallbacks
	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		RendererModule->GetResolvedSceneColorCallbacks().Remove(OnPostResolvedSceneColorHandle);
	}

	OnPostResolvedSceneColorHandle.Reset();
}


void FCSclsiModule::UpdateParameters(float spring_K, float kinetic_damping, float drag_damping, float delta_t, float wind_strength)
{
	myShaderUsageConfigParameters.spring_K = spring_K;
	myShaderUsageConfigParameters.kinetic_damping = kinetic_damping;
	myShaderUsageConfigParameters.drag_damping = drag_damping;
	myShaderUsageConfigParameters.delta_t = delta_t;
	myShaderUsageConfigParameters.wind_strength = wind_strength;
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCSclsiModule, CSclsi)