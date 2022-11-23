import taichi as ti
import os

ti.init(arch=ti.vulkan)

# structure input Buffer
Thread_Num = int(1024)

Vel = ti.Vector.ndarray(3, ti.f32, Thread_Num)
Pos = ti.Vector.ndarray(3, ti.f32, Thread_Num)


@ti.kernel
def init_particles(Vel: ti.types.ndarray(field_dim=1), Pos: ti.types.ndarray(field_dim=1)):
    x_num = 50
    offs = ti.Vector([0., 200.0, 0.])
    for p in range(Thread_Num):
        Pos[p] = ti.Vector([ti.random() * 400, ti.random()* 200, ti.random() * 200]) + offs
        Vel[p] = ti.Vector([0., 0., 0.])


@ti.kernel
def MainCompute(Vel: ti.types.ndarray(field_dim=1), Pos: ti.types.ndarray(field_dim=1)):
    Range = 800
    ParticleRadius = 20
    Stiffness = 6e3
    RestitutionCoef = 1e-4
    delta_t = 5e-4
    K = Stiffness
    C = 2. * (1. / ti.sqrt(1. + (3.14 / ti.log(RestitutionCoef)) ** 2)) * ti.sqrt(K / 2)
    collisionDistance = 2 * ParticleRadius
    range = Range / 2

    for i in ti.ndrange(Pos.shape[0]):
        acc_x = ti.Vector([0., 0., 0.])

        for j in ti.ndrange(Pos.shape[0]):
            if i != j:
                neighborPos = Pos[j]
                neighborVel = Vel[j]
                rel_pos = neighborPos - Pos[i]
                distance = ti.sqrt(rel_pos[0] ** 2 + rel_pos[1] ** 2 + rel_pos[2] ** 2)
                if distance < collisionDistance:
                    normal = (neighborPos - Pos[i]) / distance
                    f1 = normal * (collisionDistance - distance) * K  # 一个刚度和距离决定的力
                    V = (neighborVel - Vel[i]) * normal
                    f2 = C * V * normal
                    acc_x += f2 - f1

        Vel[i] += delta_t * acc_x + delta_t * 1e4 * ti.Vector([0., 0., -9.8])

        # direction x
        pos = Pos[i]
        velocity = Vel[i]
        if pos[0] >= range and velocity[0] > 0:
            Vel[i][0] *= -0.6
        elif pos[0] <= -range and velocity[0] < 0:
            Vel[i][0] *= -0.6
        # direction y
        if pos[1] >= range and velocity[1] > 0:
            Vel[i][1] *= -0.6
        elif pos[1] <= -range and velocity[1] < 0:
            Vel[i][1] *= -0.6
        # direction z
        if pos[2] >= range and velocity[2] > 0:
            Vel[i][2] *= -0.6
        elif pos[2] <= -range and velocity[2] < 0:
            Vel[i][2] *= -0.6

    for i in ti.ndrange(Pos.shape[0]):
        Pos[i] += delta_t * Vel[i]


def mainAOT():
    sym_Vel = ti.graph.Arg(ti.graph.ArgKind.NDARRAY, 'Vel', ti.f32, field_dim=1, element_shape=(3,))
    sym_Pos = ti.graph.Arg(ti.graph.ArgKind.NDARRAY, 'Pos', ti.f32, field_dim=1, element_shape=(3,))

    g_demo_builder = ti.graph.GraphBuilder()

    # g_demo_builder.dispatch(init_particles, sym_Vel, sym_Pos)
    g_demo_builder.dispatch(MainCompute, sym_Vel, sym_Pos)

    g_demo = g_demo_builder.compile()

    mod = ti.aot.Module(arch=ti.vulkan)
    mod.add_graph('MainCompute', g_demo)
    save_dir = "./Taichi_Free_AOT"
    os.makedirs(save_dir, exist_ok=True)
    mod.save(save_dir, '')


if __name__ == "__main__":
    ti.init(arch=ti.vulkan)
    mainAOT()
