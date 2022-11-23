import taichi as ti
from Taichi_ClothSimulation import *

substeps = int(1/120//delta_t)
indices = ti.field(int, shape=(NUM_THREAD_X-1) * (NUM_THREAD_Y-1) * 6) # mesh index. One guad-six index-two triangles
vertices = ti.Vector.field(3, dtype=float, shape=NUM_THREAD_X*NUM_THREAD_Y)
colors = ti.Vector.field(3, dtype=float, shape=NUM_THREAD_X*NUM_THREAD_Y)

"""
For JIT Rendering
"""
@ti.kernel
def initialize_mass_points():
    random_offset = ti.Vector([ti.random() - 0.5, ti.random() - 0.5]) * 0.1
    for p in ti.grouped(Pos):
        Pos[p] = [p[0] * quad_size - 0.5 + random_offset[0], 0.6, p[1] * quad_size - 0.5 + random_offset[1]]
        Vel[p] = [0., 0., 0.]


"""
Create Cloth Mesh
"""
@ti.kernel
def initialize_mesh_indices():
    for i, j in ti.ndrange(NUM_THREAD_X-1, NUM_THREAD_Y-1):
        quad_id = (i * (NUM_THREAD_X-1) ) + j

        indices[quad_id * 6 + 0] = i * NUM_THREAD_X + j
        indices[quad_id * 6 + 1] = (i+1) * NUM_THREAD_X + j
        indices[quad_id * 6 + 2] = i * NUM_THREAD_X + (j+1)

        indices[quad_id * 6 + 3] = (i+1) * NUM_THREAD_X + j + 1
        indices[quad_id * 6 + 4] = i * NUM_THREAD_X + j + 1
        indices[quad_id * 6 + 5] = (i+1) * NUM_THREAD_X + j

    for i, j in ti.ndrange(NUM_THREAD_X, NUM_THREAD_Y):
        if (i//4 + j//4) % 2 == 0:
            colors[i * NUM_THREAD_X + j] = (0., 0.5, 1.)
        else:
            colors[i * NUM_THREAD_X + j] = (1., 0.5, 0.)


@ti.kernel
def transition_vertices():
    for i, j in ti.ndrange(NUM_THREAD_X, NUM_THREAD_Y):
        vertices[i * NUM_THREAD_X + j] = Pos[i, j]


def main():
    window = ti.ui.Window("TaichiCLothSimulationRender", (1024,1024), vsync=True)
    canvas = window.get_canvas()
    canvas.set_background_color((0., 0., 0.))
    scene = ti.ui.Scene()
    camera = ti.ui.Camera()
    camera.position(0.0, 0.0, 3)
    camera.lookat(0.0, 0.0, 0)
    camera.track_user_inputs(window, movement_speed=0.3, hold_key=ti.ui.RMB)
    scene.set_camera(camera)
    initialize_mesh_indices()
    initialize_mass_points()
    current_time = 0.

    while window.running:
        if current_time > 0.03:
            initialize_mass_points()
            current_time = 0.0
        current_time += delta_t

        for i in range(substeps):
            mainCompute()
        transition_vertices()

        scene.point_light(pos=(0, 1, 2), color=(1, 1, 1))
        scene.mesh(vertices, indices=indices, per_vertex_color=colors, two_sided=True)
        # scene.particles(ball_center,radius=ball_radius*0.95, color =(0, 0.6, 0))
        canvas.scene(scene)
        window.show()


if __name__ == "__main__":
    print(substeps)
    main()