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
        1024,  // shadow_size
        0.1,  // shadow_min_dist
        1000.0,  // shadow_max_dist
        0.005,  // shadow_disk_radius
        0.001,  // shadow_bias
        64  // shadow_n_samples
    );
}

void create_point_light(PointLight* point_light) {
    Vec3 point_light_world_pos = {{0.0, 8.0, -20.0}};
    Vec3 point_light_color = {{1.0, 1.0, 1.0}};
    point_light_create(
        point_light,
        point_light_world_pos,
        point_light_color,
        200.0  // energy
    );
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

    Vec3 plane_scale = {{1000.0, 1000.0, 1000.0}};
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

    PointLight point_light;
    create_point_light(&point_light);

    Renderer renderer;
    create_renderer(&renderer);

    size_t n_meshes = 10;
    Mesh meshes[n_meshes];
    create_meshes(meshes, n_meshes);

    while (!glfwWindowShouldClose(window.glfw_window)) {
        renderer_set_scene(&renderer, camera, point_light);
        for (size_t i = 0; i < n_meshes; ++i) {
            renderer_draw_shadows(&renderer, &meshes[i]);
            if (i != n_meshes - 1) {
                mesh_rotate(&meshes[i], 0.01, 0.01, 0.01);
            }
        }
        for (size_t i = 0; i < n_meshes; ++i) {
            renderer_draw_materials(&renderer, &meshes[i], SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        glfwSwapBuffers(window.glfw_window);
        glfwPollEvents();
    }

    glfwTerminate();
}

