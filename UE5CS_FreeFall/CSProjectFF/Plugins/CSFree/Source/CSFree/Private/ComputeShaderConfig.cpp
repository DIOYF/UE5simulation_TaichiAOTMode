
#include "ComputeShaderConfig.h"
#include "CSFree.h"
#include "Interfaces/IPluginManager.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ShaderParameterStruct.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"
#include"RHI.h"


#define NUM_THREADS_PER_GROUP_DIMENSION 32


// Shader Config
class FComputeShaderConfigCS:public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FComputeShaderConfigCS)
	SHADER_USE_PARAMETER_STRUCT(FComputeShaderConfigCS, FGlobalShader)
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWStructuredBuffer<float3>, Vel)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<float3>, Pos)
		SHADER_PARAMETER(float, Range)
		SHADER_PARAMETER(float, ParticleRadius)
		SHADER_PARAMETER(float, Stiffness)
		SHADER_PARAMETER(float, RestitutionCoef)
		SHADER_PARAMETER(float, delta_t)
	
	END_SHADER_PARAMETER_STRUCT()


public:
	// Thread Set
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
	}

};


IMPLEMENT_GLOBAL_SHADER(FComputeShaderConfigCS, "/CSFree/Private/CS_Free.usf", "MainCompute", SF_Compute);


void FCSFreeModule::RunComputeShader_RenderThread(FRDGBuilder& builder, const FSceneTextures& SceneTextures)
{
	FRHICommandListImmediate& RHICmdList = builder.RHICmdList;
	check(IsInRenderingThread());

	FComputeShaderConfigCS::FParameters myPassParameters;
	myPassParameters.Vel = myShaderUsageConfigParameters.VelBufferUAV;
	myPassParameters.Pos = myShaderUsageConfigParameters.PosBufferUAV;
	myPassParameters.Range = myShaderUsageConfigParameters.Range;
	myPassParameters.ParticleRadius = myShaderUsageConfigParameters.ParticleRadius;
	myPassParameters.Stiffness = myShaderUsageConfigParameters.Stiffness;
	myPassParameters.RestitutionCoef = myShaderUsageConfigParameters.RestitutionCoef;
	myPassParameters.delta_t = myShaderUsageConfigParameters.delta_t;

	TShaderMapRef<FComputeShaderConfigCS> myComputeShaderConfig(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	RHICmdList.BlockUntilGPUIdle();
	FComputeShaderUtils::Dispatch(
		RHICmdList, myComputeShaderConfig, myPassParameters,
		FIntVector(FMath::DivideAndRoundUp(32, NUM_THREADS_PER_GROUP_DIMENSION),
			FMath::DivideAndRoundUp(32, NUM_THREADS_PER_GROUP_DIMENSION), 1)
	);

	// copy back to cpu
	FVector3f* srcPtr = (FVector3f*)RHILockBuffer(myShaderUsageConfigParameters.Pos, 0, arr_size * sizeof(FVector3f), EResourceLockMode::RLM_ReadOnly);

	FMemory::Memcpy(Positions.GetData(), srcPtr, arr_size * sizeof(FVector3f));
	RHIUnlockBuffer(myShaderUsageConfigParameters.Pos.GetReference());

}