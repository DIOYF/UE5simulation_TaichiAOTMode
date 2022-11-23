// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#include "RenderGraphResources.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"


struct FShaderUsageConfigParameters
{
	FStructuredBufferRHIRef Vel;
	FUnorderedAccessViewRHIRef VelBufferUAV; // UAV for writing
	FStructuredBufferRHIRef Pos;
	FUnorderedAccessViewRHIRef PosBufferUAV; // UAV for writing
	float Range = 1000;
	float ParticleRadius = 20;
	float Stiffness = 6000;
	float RestitutionCoef = 0.001;
	float delta_t = 0.005;
};


class CSFREE_API FCSFreeModule : public IModuleInterface
//class DLLEXPORT FCSFreeModule : public IModuleInterface
{
public:
	static inline FCSFreeModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FCSFreeModule>("CSFree");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("CSFree");
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	void BeginRendering();
	void EndRendering();

	void UpdateParameters(float Range,float ParticleRadius, float Stiffness, float RestitutionCoef, float delta_t);
	FShaderUsageConfigParameters myShaderUsageConfigParameters;
	TArray<FVector3f> Positions;
	int arr_size = 1024;
	

private:
	FDelegateHandle OnPostResolvedSceneColorHandle;
	//FCriticalSection RenderEveryFrameLock;
	volatile bool bCachedParametersValid;

	//FStructuredBufferRHIRef Vel;
	//FUnorderedAccessViewRHIRef VelBufferUAV; // UAV for writing
	//FStructuredBufferRHIRef Pos;
	//FUnorderedAccessViewRHIRef PosBufferUAV; // UAV for writing
	//float Range = 1000;
	//float ParticleRadius = 20;
	//float Stiffness = 6000;
	//float RestitutionCoef = 0.001;
	//float delta_t = 0.0005;
	
	// Default Setting for renderring


	void RunComputeShader_RenderThread(FRDGBuilder& RHICmdList, const FSceneTextures& SceneTextures);
	

};
