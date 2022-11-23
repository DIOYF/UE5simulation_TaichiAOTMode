import taichi as ti

ti.init(arch=ti.cuda)

NUM_THREAD_X = 128
NUM_THREAD_Y = 128

Vel = ti.Vector.field(3, dtype=float, shape=(NUM_THREAD_X, NUM_THREAD_Y))
Pos = ti.Vector.field(3, dtype=float, shape=(NUM_THREAD_X, NUM_THREAD_Y))


quad_size = 1/NUM_THREAD_X
spring_K = 3e4
kinetic_damping = 1e4
drag_damping = 1
delta_t = 6e-3 / NUM_THREAD_X
wind_force = ti.Vector([0., 0., 5.0])
gravity = ti.Vector([0., -9.8, 0.])


@ti.func
def add_acceleration(thread_x, thread_y, position, velocity):
    acc = gravity
    offsets = [[-1, -1], [-1, 0], [-1, 1], [0, -1], [0, 1], [1, -1], [1, 0], [1, 1]]
    for spring_offset in ti.static(offsets):
        x = thread_x + spring_offset[0]
        y = thread_y + spring_offset[1]
        if 0 <= x < NUM_THREAD_X and 0<= y < NUM_THREAD_Y:
            relative_pos = position - Pos[x, y]
            relative_vel = velocity - Vel[x, y]
            direction = relative_pos.normalized()
            current_distance = relative_pos.norm()
            original_distance = quad_size * ti.sqrt((thread_x-x)**2 + (thread_y-y)**2)
            # print((current_distance/original_distance - 1))
            acc += -spring_K * direction * (current_distance/original_distance - 1)
            acc += -relative_vel.dot(direction) * direction * kinetic_damping * quad_size

    return acc


@ti.kernel
def mainCompute():
    for p in ti.grouped(Pos):
            acc = add_acceleration(p[0], p[1], Pos[p], Vel[p])
            ti.sync()
            if p[1] != 0:
                Vel[p] += (wind_force + acc) * delta_t
                Vel[p] *= ti.exp(-drag_damping * delta_t)
                Pos[p] += Vel[p] * delta_t


def mainAOT():
    mainCompute()


if __name__ == "__main__":
    mainAOT()

