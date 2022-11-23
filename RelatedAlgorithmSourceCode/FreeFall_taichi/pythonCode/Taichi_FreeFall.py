import taichi as ti

ti.init(arch=ti.cuda)
# structure input Buffer
Thread_Num = 1024

Vel = ti.Vector.field(3, ti.f32, Thread_Num)
Pos = ti.Vector.field(3, ti.f32, Thread_Num)

Range = 800
ParticleRadius = 20
Stiffness = 6e3
RestitutionCoef = 1e-4
delta_t = 5e-4


@ti.func
def RangeChecker(pos, vel):
    velocity = vel
    range = Range / 2
    # direction x
    if pos[0] >= range and velocity[0]>0:
        velocity[0] *= -0.6
    elif pos[0] <= -range and velocity[0] < 0:
        velocity[0] *= -0.6
    # direction y
    if pos[1] >= range and velocity[1] > 0:
        velocity[1] *= -0.6
    elif pos[1] <= -range and velocity[1] < 0:
        velocity[1] *= -0.6
    # direction z
    if pos[2] >= range and velocity[2] > 0:
        velocity[2] *= -0.6
    elif pos[2] <= -range and velocity[2] < 0:
        velocity[2] *= -0.6
    return velocity


@ti.func
def GetAcceleration(x, position, velocity):
    K = Stiffness
    C = 2. * (1. / ti.sqrt(1. + (3.14/ti.log(RestitutionCoef))**2)) * ti.sqrt(K/2)
    collisionDistance = 2 * ParticleRadius
    acc_x = ti.Vector([0., 0., 0.])
    for i in range(Thread_Num):
        if x != i:
            neighborPos = Pos[i]
            neighborVel = Vel[i]
            rel_pos = neighborPos - position
            distance = ti.sqrt(rel_pos[0]**2 + rel_pos[1]**2 + rel_pos[2]**2)
            if distance < collisionDistance:
                normal = (neighborPos - position) / distance
                f1 = normal * (collisionDistance-distance) * K  # 一个刚度和距离决定的力
                V = (neighborVel - velocity) * normal
                f2 = C * V * normal
                acc_x += f2 - f1
    return acc_x


@ti.kernel
def MainCompute():
    for i in range(Thread_Num):
        Vel[i] += delta_t * GetAcceleration(i, Pos[i], Vel[i]) + delta_t * 1e4 * ti.Vector([0., 0., -9.8])
        Vel[i] = RangeChecker(Pos[i] + delta_t * Vel[i], Vel[i])

    for i in range(Thread_Num):
        Pos[i] += delta_t * Vel[i]


def mainAOT():
    m = ti.aot.Module(ti.cuda)
    dir_name = "./aot_module_generated_files"
    m.save(dir_name, '')


if __name__ == "__main__":
    mainAOT()