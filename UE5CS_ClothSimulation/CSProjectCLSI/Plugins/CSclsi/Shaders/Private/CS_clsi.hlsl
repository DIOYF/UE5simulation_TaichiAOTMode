#pragma once

#define THREAD_NUM_X 128
#define THREAD_NUM_Y 128

RWStructuredBuffer<float3> Vel;
RWStructuredBuffer<float3> Pos;

float spring_K = 3e4;
float kinetic_damping = 1e4;
float drag_damping = 1;
float delta_t = 6e-3 ;
float wind_strength = 0.0;


float3 add_acceleration(int thread_x, int thread_y, float3 position, float3 velocity)
{
    float quad_size = 1.0f / 128.0f;
    float3 acc = (0.f, 0.f, 0.f);
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
                
                if (0 <= x && x< THREAD_NUM_X && 0 <= y && y< THREAD_NUM_Y)
                {
                    // 有问题的点，thread_x = 0, thread_x = 127
                    float3 relative_pos = position - Pos[x * THREAD_NUM_Y + y];
                    float3 relative_vel = velocity - Vel[x * THREAD_NUM_Y + y];
                    float3 direction = normalize(relative_pos);
                    
                    float current_distance = length(relative_pos);
                    float original_distance = quad_size * length(float2(i * 1.0f, j * 1.0f));
                    
                    acc += -spring_K * direction * (current_distance / original_distance - 1);
                    acc += -dot(relative_vel, direction) * direction * kinetic_damping * quad_size;
                }
            }
        }
    }
    return acc ;
}


//float substeps = int(floor( 0.008f / delta_t));
[numthreads(THREADGROUPSIZE_X, THREADGROUPSIZE_Y, THREADGROUPSIZE_Z)]
void mainCompute(uint3 ThreadID : SV_DispatchThreadID)
{
    int index = (ThreadID.x) * THREAD_NUM_Y + (ThreadID.y);
    float dt = delta_t / THREAD_NUM_X ;
    float3 gravity = float3(0., 0., -9.8f);
    
    for (int i = 0; i < int(floor(( 1.0 / 120.0) / delta_t) ); i++)
    {
        float3 acc = gravity + float3(0., wind_strength, 0.) + add_acceleration(ThreadID.x, ThreadID.y, Pos[index], Vel[index]) ;
        //float3 acc = gravity;
        GroupMemoryBarrierWithGroupSync();
        if (ThreadID.y != 0 )
        {
            Vel[index] += acc * 20 * dt;
            Vel[index] *= exp(-drag_damping * dt);
            Pos[index] += Vel[index] * dt;
        }
       
    }
    //Pos[index] = float3(ThreadID.x, ThreadID.y, ThreadID.z);

}