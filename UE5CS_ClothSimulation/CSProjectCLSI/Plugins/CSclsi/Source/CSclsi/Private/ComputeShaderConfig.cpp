#include "ComputeShaderConfig.h"
#include "CSclsi.h"
#include "Interfaces/IPluginManager.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"
#include "UniformBuffer.h"
#include "Shader.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RHIStaticStates.h"
#include "RHICommandList.h"
#include "RHI.h"

#define NUM_THREADS_PER_GROUP_DIMENSION 32

// Shader Config

class FComputeShaderConfig : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FComputeShaderConfig)
	SHADER_USE_PARAMETER_STRUCT(FComputeShaderConfig, FGlobalShader)
		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWStructuredBuffer<float3>, Vel)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<float3>, Pos)
		SHADER_PARAMETER(float, spring_K)
		SHADER_PARAMETER(float, kinetic_damping)
		SHADER_PARAMETER(float, drag_damping)
		SHADER_PARAMETER(float, delta_t)
		SHADER_PARAMETER(float, wind_strength)
	END_SHADER_PARAMETER_STRUCT()

public:
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


IMPLEMENT_GLOBAL_SHADER(FComputeShaderConfig, "/CSclsi/Private/CS_clsi.usf", "mainCompute", SF_Compute);





void FCSclsiModule::RunComputeShader_RenderThread(FRDGBuilder& builder, const FSceneTextures& SceneTextures)
{
	FRHICommandListImmediate& RHICmdList = builder.RHICmdList;
	check(IsInRenderingThread());

	FComputeShaderConfig::FParameters myPassParameters;
	myPassParameters.Vel = myShaderUsageConfigParameters.VelBufferUAV;
	myPassParameters.Pos = myShaderUsageConfigParameters.PosBufferUAV;
	myPassParameters.spring_K = myShaderUsageConfigParameters.spring_K;
	myPassParameters.kinetic_damping = myShaderUsageConfigParameters.kinetic_damping;
	myPassParameters.drag_damping = myShaderUsageConfigParameters.drag_damping;
	myPassParameters.delta_t = myShaderUsageConfigParameters.delta_t;
	myPassParameters.wind_strength = myShaderUsageConfigParameters.wind_strength;


	TShaderMapRef<FComputeShaderConfig> myComputeShaderConfig(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	RHICmdList.BlockUntilGPUIdle();
	FComputeShaderUtils::Dispatch(
		RHICmdList, myComputeShaderConfig, myPassParameters,
		FIntVector(FMath::DivideAndRoundUp(128, NUM_THREADS_PER_GROUP_DIMENSION),
			FMath::DivideAndRoundUp(128, NUM_THREADS_PER_GROUP_DIMENSION), 1)
	);


	FVector3f* srcPtr = (FVector3f*)RHILockBuffer(myShaderUsageConfigParameters.Pos, 0, arr_size_x * arr_size_y * sizeof(FVector3f), EResourceLockMode::RLM_ReadOnly);

	FMemory::Memcpy(Positions.GetData(), srcPtr, arr_size_x * arr_size_y * sizeof(FVector3f));
	RHIUnlockBuffer(myShaderUsageConfigParameters.Pos.GetReference());

}






