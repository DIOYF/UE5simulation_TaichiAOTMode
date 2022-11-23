#pragma once

#include "CudaClsi.h"
//#include "device_functions.h"
#include <iostream>

// CUDA RUNTIME API
__device__ float3 Vel[THREAD_NUM_X * THREAD_NUM_Y];
__device__ float3 Pos[THREAD_NUM_X * THREAD_NUM_Y];
__device__ float spring_K;
__device__ float kinetic_damping;
__device__ float drag_damping;
__device__ float delta_t_1;
__device__ float3 wind_force;


__global__ void mainCompute_1();
//__device__ float3 Acc[THREAD_NUM_X * THREAD_NUM_Y];


__device__ float3 add_acceleration(int thread_x, int thread_y,float3 position, float3 velocity)
{
    float quad_size = 1.0f / 128.0f;
    float3 acc = { 0.f,0.f, -9.8f };
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            else
            {
                int x = thread_x + i;
                int y = thread_y + j;
                if (0 <= x && x< THREAD_NUM_X && 0 <= y && y < THREAD_NUM_Y)
                {
                    float3 relative_pos = { position.x - Pos[x * THREAD_NUM_Y + y].x, position.y - Pos[x * THREAD_NUM_Y + y].y, position.z - Pos[x * THREAD_NUM_Y + y].z };
                    float3 relative_vel = { velocity.x - Vel[x * THREAD_NUM_Y + y].x, velocity.y - Vel[x * THREAD_NUM_Y + y].y, velocity.z - Vel[x * THREAD_NUM_Y + y].z };
                    //float3 direction = normal(relative_pos);

                    float current_distance = sqrtf( powf(relative_pos.x, 2) + powf(relative_pos.y, 2) + powf(relative_pos.z, 2) );
                    float original_distance = quad_size * sqrtf( powf(i * 1.0f,2) + powf(j * 1.0f,2));
                    

                    float3 direction = {relative_pos.x/current_distance, relative_pos.y/current_distance, relative_pos.z/current_distance};  

                    float K = relative_vel.x * direction.x + relative_vel.y * direction.y + relative_vel.z * direction.z;
                    
                    float relative_distance = (current_distance / original_distance - 1);

                    acc.x += -(spring_K  * relative_distance) * direction.x;
                    acc.x += -K * direction.x * kinetic_damping * quad_size;
                    
                    acc.y += -(spring_K * relative_distance)  * direction.y;
                    acc.y += -K * direction.y * kinetic_damping * quad_size;
                    
                    acc.z += -(spring_K * relative_distance)  * direction.z;
                    acc.z += -K * direction.z * kinetic_damping * quad_size;
                }
            }
        }
    }
    //if (isnan(acc.x) || isnan(acc.y) || isnan(acc.z)) return  { 0.f,0.f,-9.8f };
    return acc;
}


__global__ void mainCompute_1()
{
    int i = blockIdx.x * blockDim.x + threadIdx.x, j = blockIdx.y * blockDim.y + threadIdx.y;
    //for (int i = 0; i < 20; i++)
    {
        float3 acc = add_acceleration(i, j, Pos[i * THREAD_NUM_Y + j], Vel[i * THREAD_NUM_Y + j]);

        //__threadfence();

        if (j != 0)
        {
            //float3 acc = Acc[i * THREAD_NUM_Y + j];
            Vel[i * THREAD_NUM_Y + j].x +=  (wind_force.x + acc.x) * delta_t_1;
            Vel[i * THREAD_NUM_Y + j].y +=  (wind_force.y + acc.y) * delta_t_1;
            Vel[i * THREAD_NUM_Y + j].z +=  (wind_force.z + acc.z) * delta_t_1;

            Vel[i * THREAD_NUM_Y + j].x *= exp(-drag_damping * delta_t_1);
            Vel[i * THREAD_NUM_Y + j].y *= exp(-drag_damping * delta_t_1);
            Vel[i * THREAD_NUM_Y + j].z *= exp(-drag_damping * delta_t_1);

            Pos[i * THREAD_NUM_Y + j].x += Vel[i * THREAD_NUM_Y + j].x * delta_t_1;
            Pos[i * THREAD_NUM_Y + j].y += Vel[i * THREAD_NUM_Y + j].y * delta_t_1;
            Pos[i * THREAD_NUM_Y + j].z += Vel[i * THREAD_NUM_Y + j].z * delta_t_1;
        }
    }
}


__global__ void mainCompute_2()
{
    int i = blockIdx.x * blockDim.x + threadIdx.x, j = blockIdx.y * blockDim.y + threadIdx.y;

}


// Compiled Lib Function Using for freefall compute
cudaError_t ClothSimulationCompute(metadata myMetaData, float3* positions, float3* velocities, std::string* error_message)
{
    float spring_K_value = myMetaData.spring_K;
    float kinetic_damping_value = myMetaData.kinetic_damping;
    float drag_damping_value = myMetaData.drag_damping;
    float delta_t_value = myMetaData.delta_t / THREAD_NUM_X;
    float3 wind_force_value = { myMetaData.wind_strength * 0.f, myMetaData.wind_strength * 0.f, myMetaData.wind_strength * 0.f };
    
    cudaError_t cuda_status;
    cuda_status = cudaSetDevice(0);
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaSetDevice failed!";
        goto Error;
    }

    cuda_status = cudaMemcpyToSymbol(Vel, velocities, sizeof(float3) * THREAD_NUM_X * THREAD_NUM_Y);
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol1 failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyToSymbol(Pos, positions, sizeof(float3) * THREAD_NUM_X * THREAD_NUM_Y);
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol2 failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyToSymbol(spring_K, &spring_K_value, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol3 failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyToSymbol(kinetic_damping, &kinetic_damping_value, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol4 failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyToSymbol(drag_damping, &drag_damping_value, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol5 failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyToSymbol(delta_t_1, &delta_t_value, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol6 failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyToSymbol(wind_force, &wind_force_value, sizeof(float3));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyToSymbol7 failed!";
        goto Error;
    }

    dim3 threadPerBlock = {32,32,1};
    dim3 blockPerGrid ={128/threadPerBlock.x, 128/threadPerBlock.y,1};
    // How to align Thread is what we need consider
    //MainCompute << < blockPerGrid, threadPerBlock >> > ();

    mainCompute_1 <<< blockPerGrid, threadPerBlock , 3 >> > ();

    cuda_status = cudaMemcpyFromSymbol(velocities, Vel, sizeof(float3) * THREAD_NUM_X * THREAD_NUM_Y);
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol1 failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyFromSymbol(positions, Pos, sizeof(float3) * THREAD_NUM_X * THREAD_NUM_Y);
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol2 failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyFromSymbol(&spring_K_value, spring_K, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol3 failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyFromSymbol(&kinetic_damping_value, kinetic_damping, sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol4 failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyFromSymbol(&drag_damping_value, drag_damping,  sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol5 failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyFromSymbol(&drag_damping_value, delta_t_1,  sizeof(float));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol6 failed!";
        goto Error;
    }
    cuda_status = cudaMemcpyFromSymbol(&wind_force_value, wind_force,  sizeof(float3));
    if (cuda_status != cudaSuccess) {
        *error_message = "cudaMemcpyFromSymbol7 failed!";
        goto Error;
    }


Error:
    cudaFree(Vel);
    cudaFree(Pos);
    cudaFree(&spring_K);
    cudaFree(&kinetic_damping);
    cudaFree(&drag_damping);
    cudaFree(&delta_t_1);
    cudaFree(&wind_force);

    return cuda_status;
}
