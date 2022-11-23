import taichi as ti
import os

ti.init(arch=ti.vulkan)

NUM_THREAD_X = 128
NUM_THREAD_Y = 128

Vel = ti.Vector.ndarray(3, dtype=float, shape=(NUM_THREAD_X * NUM_THREAD_Y))
Pos = ti.Vector.ndarray(3, dtype=float, shape=(NUM_THREAD_X * NUM_THREAD_Y))


# @ti.kernel
# def initialize_mass_points(Vel: ti.types.ndarray(field_dim=1), Pos: ti.types.ndarray(field_dim=1)):
#     quad_size = 1 / NUM_THREAD_X
#     random_offset = ti.Vector([ti.random() - 0.5, ti.random() - 0.5]) * 0.1
#     for p in ti.grouped(Pos):
#         Pos[p] = [p[0] * quad_size - 0.5 + random_offset[0], 0.6, p[1] * quad_size - 0.5 + random_offset[1]]
#         Vel[p] = [0., 0., 0.]


@ti.kernel
def mainCompute(Vel: ti.types.ndarray(field_dim=1), Pos: ti.types.ndarray(field_dim=1)):
    quad_size = 1 / NUM_THREAD_X
    spring_K = 3e4
    kinetic_damping = 1e4
    drag_damping = 1
    delta_t = 6e-3 / NUM_THREAD_X
    wind_force = ti.Vector([0., 0., 5.0])
    gravity = ti.Vector([0., 0., -9.8])

    for m,n in ti.ndrange(128, 128):
        acc = gravity
        offsets = [[-1, -1], [-1, 0], [-1, 1], [0, -1], [0, 1], [1, -1], [1, 0], [1, 1]]
        for spring_offset in ti.static(offsets):
            x = m + spring_offset[0]
            y = n + spring_offset[1]
            if 0 <= x < NUM_THREAD_X and 0 <= y < NUM_THREAD_Y:
                relative_pos = Pos[m * NUM_THREAD_Y + n] - Pos[x * NUM_THREAD_Y + y]
                relative_vel = Vel[m * NUM_THREAD_Y + n] - Vel[x * NUM_THREAD_Y + y]
                direction = relative_pos.normalized()
                current_distance = relative_pos.norm()
                original_distance = quad_size * ti.sqrt((m - x) ** 2 + (n - y) ** 2)
                # print((current_distance/original_distance - 1))
                acc += -spring_K * direction * (current_distance / original_distance - 1)
                acc += -relative_vel.dot(direction) * direction * kinetic_damping * quad_size

        if n != 0:
            Vel[m * NUM_THREAD_Y + n] += (wind_force + acc) * delta_t
            Vel[m * NUM_THREAD_Y + n] *= ti.exp(-drag_damping * delta_t)
            Pos[m * NUM_THREAD_Y + n] += Vel[m * NUM_THREAD_Y + n] * delta_t


def mainAOT():
    sym_Vel = ti.graph.Arg(ti.graph.ArgKind.NDARRAY, 'Vel', ti.f32, field_dim=1, element_shape=(3,))
    sym_Pos = ti.graph.Arg(ti.graph.ArgKind.NDARRAY, 'Pos', ti.f32, field_dim=1, element_shape=(3,))

    g_demo_builder = ti.graph.GraphBuilder()
    g_demo_builder.dispatch(mainCompute, sym_Vel, sym_Pos)

    g_demo = g_demo_builder.compile()

    mod = ti.aot.Module(arch=ti.vulkan)
    mod.add_graph('MainCompute', g_demo)
    save_dir = "./Taichi_clsi_AOT"
    os.makedirs(save_dir, exist_ok=True)
    mod.save(save_dir, '')


if __name__ == "__main__":
    mainAOT()

