#pragma once

// #include "/Engine/Public/Platform.ush"
#define THREAD_NUM 1024

RWStructuredBuffer<float3> Vel;
RWStructuredBuffer<float3> Pos;

float Range ;
float ParticleRadius;
float Stiffness;
float RestitutionCoef;
float delta_t;


float3 RangeChecker(float3 pos, float3 vel)
{
    float3 velocity = vel;
    float range = Range / 2;
    // direction x
    if ( pos.x >= range && velocity.x > 0 )
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


float3 getAcceleration(int index, float3 position, float3 velocity)
{
    float K = Stiffness;
    float C = 2. * (1. / sqrt(1. + pow(3.14 / log(RestitutionCoef), 2))) * sqrt(K/2);
    float collisionDistance = 2 * ParticleRadius;
    float3 acc_x = float3(0.f, 0.f, 0.f);
    for (int i = 0; i < THREAD_NUM; i++)
    {
        if( index != i )
        {
            float3 neighborPos = Pos[i];
            float3 neighborVel = Vel[i];
            float3 rel_pos = neighborPos - position;
            float distance = length(rel_pos);
            if (distance < collisionDistance)
            {
                float3 normal = rel_pos / distance;
                float3 f1 = normal * (collisionDistance - distance) * K;
                float V = dot(neighborVel - velocity,normal);
                float3 f2 = C * V * normal;
                acc_x = f2 - f1;
            }
        }
    }
    return acc_x;
}


[numthreads(THREAD_NUM, 1, 1)]
void MainCompute(uint3 ThreadID: SV_DispatchThreadID)
{
    int index = ThreadID.x;
    Vel[index] += delta_t * getAcceleration(index, Pos[index], Vel[index]) + delta_t * 1e4 * float3(0., 0., -9.8);
    Vel[index] = RangeChecker(Pos[index], Vel[index]);
    
    GroupMemoryBarrierWithGroupSync();
    
    Pos[index] += delta_t * Vel[index];
}