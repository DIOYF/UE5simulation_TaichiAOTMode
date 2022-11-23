#pragma once

// CUDA API Related Header File
#include "cuda_runtime.h"
#include "vector_types.h"
#include "vector_functions.h"
#include "device_launch_parameters.h"
#include <string>
#define THREAD_NUM 1024


struct metadata
{
    float Range;
    float ParticleRadius;
    float Stiffness;
    float RestitutionCoef;
    float delta_t;
};


cudaError_t FreeFallCompute(metadata myMetaData, float3* Pos, float3* Vel, std::string* error_message);