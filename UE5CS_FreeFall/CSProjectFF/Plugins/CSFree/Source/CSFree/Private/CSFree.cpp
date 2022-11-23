// Copyright Epic Games, Inc. All Rights Reserved.

#include "CSFree.h"
#include "Interfaces/IPluginManager.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"

#include "RenderGraphBuilder.h"
#include"RHI.h"

#define LOCTEXT_NAMESPACE "FCSFreeModule"

void FCSFreeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PluginShaderDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Plugins/CSFree/Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/CSFree"), PluginShaderDir);
}


void FCSFreeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}




void FCSFreeModule::BeginRendering()
{
	if (OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}

	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		OnPostResolvedSceneColorHandle = RendererModule->GetResolvedSceneColorCallbacks().AddRaw(this, &FCSFreeModule::RunComputeShader_RenderThread);
	}

	const size_t size = sizeof(FVector3f);

	TResourceArray<FVector3f> positionArray;
	TResourceArray<FVector3f> velocityArray;

	const FVector3f init_vector(0.0f, 0.0f, 0.0f);
	float range = myShaderUsageConfigParameters.Range/2 - myShaderUsageConfigParameters.ParticleRadius*2;

	for (int i = 0; i < arr_size; i++)
	{
		
		float pos_x = FMath::RandRange(-range, range);
		float pos_y = FMath::RandRange(-range, range);
		float pos_z = FMath::RandRange(100.f, range);

		positionArray.Add(FVector3f(pos_x, pos_y, pos_z));
		Positions.Add(FVector3f(pos_x, pos_y, pos_z));
		velocityArray.Add(FVector3f(0.1f,FMath::RandRange(-0.1f,0.1f),0.f));
	}

	FRHIResourceCreateInfo createInfo(TEXT("Position_Array"));
	createInfo.ResourceArray = &positionArray;
	myShaderUsageConfigParameters.Pos = RHICreateStructuredBuffer(size, size * arr_size, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
	myShaderUsageConfigParameters.PosBufferUAV = RHICreateUnorderedAccessView(myShaderUsageConfigParameters.Pos, false, false);

	createInfo.ResourceArray = &velocityArray;
	myShaderUsageConfigParameters.Vel = RHICreateStructuredBuffer(size, size * arr_size, BUF_UnorderedAccess | BUF_ShaderResource, createInfo);
	myShaderUsageConfigParameters.VelBufferUAV = RHICreateUnorderedAccessView(myShaderUsageConfigParameters.Vel, false, false);

}



void FCSFreeModule::EndRendering()
{
	//If the handle is not valid then there's no cleanup to do
	if (!OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}
	//Get the Renderer Module and remove our entry from the ResolvedSceneColorCallbacks
	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		RendererModule->GetResolvedSceneColorCallbacks().Remove(OnPostResolvedSceneColorHandle);
	}

	OnPostResolvedSceneColorHandle.Reset();
}


void FCSFreeModule::UpdateParameters(float Range, float ParticleRadius, float Stiffness, float RestitutionCoef, float delta_t)
{
	myShaderUsageConfigParameters.Range = Range;
	myShaderUsageConfigParameters.ParticleRadius =  ParticleRadius;
	myShaderUsageConfigParameters.Stiffness = Stiffness;
	myShaderUsageConfigParameters.RestitutionCoef = RestitutionCoef;
	myShaderUsageConfigParameters.delta_t = delta_t;
}



#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCSFreeModule, CSFree)