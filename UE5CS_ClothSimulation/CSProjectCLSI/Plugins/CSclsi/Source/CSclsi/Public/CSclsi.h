// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

#include "RenderGraphResources.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"


struct FShaderUsageConfigParameters
{
	FStructuredBufferRHIRef Vel;
	FUnorderedAccessViewRHIRef VelBufferUAV;
	FStructuredBufferRHIRef Pos;
	FUnorderedAccessViewRHIRef PosBufferUAV;
	float spring_K = 3e4;
	float kinetic_damping = 1e4;
	float drag_damping = 1;
	float delta_t = 6e-3;
	float wind_strength = 0.0;
};


class CSCLSI_API FCSclsiModule : public IModuleInterface
{
public:
	// as API
	static inline FCSclsiModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FCSclsiModule>("CSclsi");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("CSclsi");
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	void BeginRendering();
	void EndRendering();

	void UpdateParameters(float spring_K, float kinetic_damping, float drag_damping, float delta_t, float wind_strength);
	FShaderUsageConfigParameters myShaderUsageConfigParameters;
	// we need consider how to transfer data to 2D mesh grid or material .mat
	int arr_size_x = 128;
	int arr_size_y = 128;
	
	TArray<FVector3f> Positions;  // It may be not proper way to denotes 2D position and velocity data mesh




private:
	FDelegateHandle OnPostResolvedSceneColorHandle;
	volatile bool bCachedParametersValid;

    void RunComputeShader_RenderThread(FRDGBuilder& RHICmdList, const FSceneTextures& SceneTextures);

};
