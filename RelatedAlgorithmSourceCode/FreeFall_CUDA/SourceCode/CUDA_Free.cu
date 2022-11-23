#pragma once

#include "CUDA_Free.h"
#include<iostream>

// CUDA RUNTIME API
__device__ float3 Vel[THREAD_NUM];
__device__ float3 Pos[THREAD_NUM];
__device__ float Range;
__device__ float ParticleRadius;
__device__ float Stiffness;
__device__ float RestitutionCoef;
__device__ float delta_t;
__global__ void MainCompute();


__device__ float3 RangeChecker(float3 pos, float3 vel)
{
    float3 velocity = vel;
    float range = Range / 2;
    // direction x
    if (pos.x >= range && velocity.x > 0)
    {
        velocity.x *= -0.6;
    }
    else if (pos.x <= -range && velocity.x < 0)
    {
        velocity.x *= -0.6;
    }
    // direction y
    if (pos.y >= range && velocity.y > 0)
    {
        velocity.y *= -0.6;
    }
    else if (pos.y <= -range && velocity.y < 0)
    {
        velocity.y *= -0.6;
    }
    // direction z
    if (pos.z >= range && velocity.z > 0)
    {
        velocity.z *= -0.6;
    }
    else if (pos.z <= -range && velocity.z < 0)
    {
        velocity.z *= -0.6;
    }
    return velocity;
}


__device__ float3 getAcceleration(int index, float3 position, float3 velocity)
{
    float K = Stiffness;
    float C = 2. * (1. / sqrt(1. + pow(3.14 / log(RestitutionCoef), 2))) * sqrt(K / 2);
    float collisionDistance = 2 * ParticleRadius;
    float3 acc_x = { 0.f, 0.f, 0.f };
    for (int i = 0; i < THREAD_NUM; i++)
    {
        if (index != i)
        {
            float3 neighborPos = Pos[i];
            float3 neighborVel = Vel[i];
            float3 rel_pos = { neighborPos.x - position.x, neighborPos.y - position.y, neighborPos.z - position.z };
            float distance = sqrt(pow(rel_pos.x, 2) + pow(rel_pos.y, 2) + pow(rel_pos.z, 2));
            if (distance < collisionDistance)
            {
                float3 normal = { rel_pos.x / distance, rel_pos.y/distance, rel_pos.z/distance};
                float3 f1 = { normal.x * (collisionDistance - distance) * K, normal.y * (collisionDistance - distance) * K, normal.z * (collisionDistance - distance) * K};
                float V = (neighborVel.x - velocity.x * normal.x)+(neighborVel.y - velocity.y * normal.y)+(neighborVel.z - velocity.z * normal.z);
                float3 f2 = { C * V * normal.x, C * V * normal.y, C * V * normal.z };
                acc_x = {acc_x.x + f2.x-f1.x, acc_x.y + f2.y-f1.y, acc_x.z + f2.z-f1.z};
            }
        }
    }
    return acc_x;
}


__global__ void MainCompute()
{
    int i = threadIdx.x;
    float3 gravity = {0., 0., -9.8};
    Vel[i].x = Vel[i].x + delta_t * getAcceleration(i, Pos[i], Vel[i]).x + delta_t * 1e4 * gravity.x;
    Vel[i].y = Vel[i].y + delta_t * getAcceleration(i, Pos[i], Vel[i]).y + delta_t * 1e4 * gravity.y;
    Vel[i].z = Vel[i].z + delta_t * getAcceleration(i, Pos[i], Vel[i]).z + delta_t * 1e4 * gravity.z;
    Vel[i] = RangeChecker(Pos[i], Vel[i]);

    Pos[i].x = Pos[i].x + delta_t * Vel[i].x;
    Pos[i].y = Pos[i].y + delta_t * Vel[i].y;
    Pos[i].z = Pos[i].z + delta_t * Vel[i].z;
}


// Compiled Lib Function Using For FreeFall Compute
cudaError_t FreeFallCompute(metadata myMetaData, float3* positions, float3* velocities, std::string* error_message)
{
    float RangeValue = myMetaData.Range;
    float ParticleRadiusValue = myMetaData.ParticleRadius;
    float StiffnessValue = myMetaData.Stiffness;
    float RestitutionCoefValue = myMetaData.RestitutionCoef;
    float delta_tValue = myMetaData.delta_t;

    cudaError_t cuda_status;
    // Choose GPU Device 0
    cuda_status = cudaSetDevice(0);
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaSetDevice failed!";
        goto Error;
    }

    // Transfer Global Data from CPU to GPU
    cuda_status = cudaMemcpyToSymbol(Vel, velocities, sizeof(float3) * THREAD_NUM);
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol Vel failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyToSymbol(Pos, positions, sizeof(float3) * THREAD_NUM);
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol Pos failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyToSymbol(Range, &RangeValue, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol Range failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyToSymbol(ParticleRadius, &ParticleRadiusValue, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol ParticleRadius failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyToSymbol(Stiffness, &StiffnessValue, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol Stiffness failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyToSymbol(RestitutionCoef, &RestitutionCoefValue, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol RestitutionCoef failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyToSymbol(delta_t, &delta_tValue, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol delta_t failed!";
        goto Error;
    }

    // Using CUDA Kernel to Compute
    MainCompute << <1, THREAD_NUM, 1 >> > ();

    // Transfer Global Data from GPU to CPU
    cuda_status = cudaMemcpyFromSymbol(velocities, Vel, sizeof(float3) * THREAD_NUM);
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol Vel failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyFromSymbol(positions, Pos, sizeof(float3) * THREAD_NUM);
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol Pos failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyFromSymbol(&RangeValue, Range, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol Range failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyFromSymbol(&ParticleRadiusValue, ParticleRadius, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol ParticleRadius failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyFromSymbol(&StiffnessValue, Stiffness, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol Stiffness failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyFromSymbol(&RestitutionCoefValue, RestitutionCoef, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol RestitutionCoef failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyFromSymbol(&delta_tValue, delta_t, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol delta_t failed!";
        goto Error;
    }

Error:
    cudaFree(Vel);
    cudaFree(Pos);
    cudaFree(&Range);
    cudaFree(&ParticleRadius);
    cudaFree(&Stiffness);
    cudaFree(&RestitutionCoef);
    cudaFree(&delta_t);

    return cuda_status;
}





