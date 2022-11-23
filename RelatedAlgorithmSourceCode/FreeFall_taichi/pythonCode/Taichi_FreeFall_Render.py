# _____________________________________for Render _ JIT MODE_________________________________________
import taichi as ti
from Taichi_FreeFall import *


Col = ti.Vector.field(4, float, Thread_Num)
Pos_proj = ti.Vector.field(3, float, Thread_Num)


@ti.kernel
def init_particles():
    x_num = 50
    offs = ti.Vector([0., 200.0, 0.])
    for p in range(Thread_Num):
        Col[p] = ti.Vector([ti.random(ti.f32), ti.random(ti.f32), ti.random(ti.f32), 1.0])
        Pos[p] = ti.Vector([ti.random() * 400, ti.random()* 200, ti.random() * 200]) + offs
        Vel[p] = ti.Vector([0., 0., 0.])


@ti.kernel
def get_pos():
    for i in range(Thread_Num):
        Pos_proj[i] = Pos[i] * 2 / Range


def mainJIT():
    # for render
    window = ti.ui.Window("freeFall", (Range, Range), vsync=True)
    canvas = window.get_canvas()
    scene = ti.ui.Scene()
    camera = ti.ui.Camera()
    camera.position(1.5, -1.5, 1.5)
    camera.lookat(0, 1.5, -1.2)
    camera.fov(70)
    init_particles()

    count = 1
    while window.running:
        MainCompute()
        get_pos()
        camera.track_user_inputs(window, movement_speed=0.3, hold_key=ti.ui.RMB)
        scene.set_camera(camera)
        scene.ambient_light((0.5, 0.5, 0.5))
        scene.point_light(pos=(0.5, 1.5, -1.0), color=(0.5, 0.5, -0.5))
        scene.point_light(pos=(0.5, 1.5, -1.5), color=(0.5, 0.5, -0.5))
        scene.set_camera(camera)
        scene.particles(Pos_proj, radius= ParticleRadius / Range, per_vertex_color=Col)
        canvas.scene(scene)
        window.show()
        count += 1


if __name__ == "__main__":
    mainJIT()
    # mainAOT()
