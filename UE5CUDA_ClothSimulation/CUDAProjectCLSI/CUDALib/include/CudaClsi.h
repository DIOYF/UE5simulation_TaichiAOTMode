#pragma once

#include "cuda_runtime.h"
#include "vector_types.h"
#include "vector_functions.h"
#include "device_launch_parameters.h"
#include <string>
#define THREAD_NUM_X 128
#define THREAD_NUM_Y 128


struct metadata
{
	float spring_K = float(3e4);
	float kinetic_damping = float(1e4);
	float drag_damping = float(1);
	float delta_t = float(6e-3) ;
	float wind_strength = 0.f;
};


cudaError_t ClothSimulationCompute(metadata myMetaData, float3* positions, float3* velocities, std::string* error_message);