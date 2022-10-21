#include "../includes.h"

size_t SCREEN_WIDTH = 1920;
size_t SCREEN_HEIGHT = 1080;

void create_camera(Camera* camera) {
    Vec3 camera_start_pos = {{0.0, 0.0, 0.0}};
    camera_create(
        camera,
        45.0,  // fov
        0.1,  // near
        1000.0,  // far
        SCREEN_WIDTH / SCREEN_HEIGHT,  // aspect
        10.0,  // side_sens
        1.0,  // straight_sens
        1.0,  // rotation_sens
        camera_start_pos
    );
}

void create_window(Window* window, Camera* camera) {
    window_create(window, SCREEN_WIDTH, SCREEN_HEIGHT, camera);
}

void create_renderer(Renderer* renderer) {
    renderer_create(
        renderer,
        512,  // shadow_size
        0.1,  // shadow_min_dist
        50.0,  // shadow_max_dist
        0.01,  // shadow_disk_radius
        0.005,  // shadow_bias_min
        0.05,  // shadow_bias_max
        16,  // shadow_min_n_samples
        64  // shadow_max_n_samples
    );
}

void create_point_lights(PointLight point_lights[], size_t n_point_lights) {
    for (size_t i = 0; i < n_point_lights; ++i) {
        Vec3 point_light_world_pos = {{i * 5, 8.0, -20.0}};
        Vec3 point_light_color = {{1.0, 1.0, 1.0}};
        point_light_create(
            &point_lights[i],
            point_light_world_pos,
            point_light_color,
            500.0  // energy
        );
    }
}

void create_meshes(Mesh meshes[], size_t n_meshes) {
    Material plane_material;
    Vec3 plane_diffuse_color = {{0.8, 0.8, 0.8}};
    Vec3 plane_ambient_color = plane_diffuse_color;
    Vec3 plane_specular_color = {{1.0, 1.0, 1.0}};
    material_create(
        &plane_material,
        plane_diffuse_color,
        plane_ambient_color,
        plane_specular_color,
        256.0  // shininess
    );

    Vec3 plane_scale = {{1000.0, 1000.0, 1.0}};
    Vec3 plane_rotation = {{-PI / 2, 0.0, 0.0}};
    Vec3 plane_translation = {{0.0, -2.0, 0.0}};
    Transformation plane_transformation = transformation_create(
        plane_scale, plane_rotation, plane_translation);
    mesh_create_plane(&meshes[n_meshes - 1], plane_material, plane_transformation);

    for (size_t i = 0; i < n_meshes - 1; ++i) {
        Material sphere_material;
        Vec3 sphere_diffuse_color = {{0.8, 0.2, 0.3}};
        Vec3 sphere_ambient_color = sphere_diffuse_color;
        Vec3 sphere_specular_color = {{1.0, 1.0, 1.0}};
        material_create(
            &sphere_material,
            sphere_diffuse_color,
            sphere_ambient_color,
            sphere_specular_color,
            256.0  // shininess
        );

        Vec3 sphere_scale = {{1.0, 1.0, 1.0}};
        Vec3 sphere_rotation = {{0.0, 0.0, 0.0}};
        Vec3 sphere_translation = {{i * 3.0, 0.0, -5.0}};
        Transformation sphere_transformation = transformation_create(
            sphere_scale, sphere_rotation, sphere_translation);

        mesh_create_icosahedron(&meshes[i], sphere_material, sphere_transformation);
    }
}


int main(void) {
    Camera camera;
    create_camera(&camera);

    Window window;
    create_window(&window, &camera);

    size_t n_point_lights = 1;
    PointLight point_lights[n_point_lights];
    create_point_lights(point_lights, n_point_lights);

    Renderer renderer;
    create_renderer(&renderer);

    size_t n_meshes = 5;
    Mesh meshes[n_meshes];
    create_meshes(meshes, n_meshes);

    bool ok = true;
    while (!glfwWindowShouldClose(window.glfw_window)) {
        renderer_set_scene(&renderer, &camera, meshes, n_meshes, point_lights, n_point_lights);
        ok &= renderer_draw_scene(&renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (!ok) {
            return 1;
        }
        // for (size_t i = 0; i < n_meshes; ++i) {
        //     if (i != n_meshes - 1) {
        //         mesh_rotate(&meshes[i], 0.01, 0.01, 0.01);
        //     }
        // }

        glfwSwapBuffers(window.glfw_window);
        glfwPollEvents();
    }

    glfwTerminate();
}

