#include "../includes.h"

size_t SCREEN_WIDTH = 1920;
size_t SCREEN_HEIGHT = 1080;

Camera* create_camera() {
    Vec3 camera_start_pos = {{0.0, 0.0, 0.0}};
    return camera_create(
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

void create_window(Window* window) {
    window_create(window, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void create_renderer(Renderer* renderer) {
    renderer_create(renderer);
}

void create_point_lights(size_t n_point_lights) {
    for (int i = 0; i < n_point_lights; ++i) {
        Vec3 point_light_world_pos = {{i * 20 * (i % 2 == 0 ? -1 : 1), 8.0, -20.0}};

        Vec3 point_light_color = {{1.0, 1.0, 1.0}};
        PointLight* point_light = point_light_create(
            point_light_world_pos,
            point_light_color,
            500.0  // energy
        );
        point_light_attach_point_shadow_caster(point_light);
    }
}

void create_meshes(Mesh meshes[], size_t n_meshes) {
    Vec3 plane_diffuse_color = {{0.8, 0.8, 0.8}};
    Vec3 plane_ambient_color = plane_diffuse_color;
    Vec3 plane_specular_color = {{1.0, 1.0, 1.0}};
    Material* plane_material = material_create(
        plane_diffuse_color,
        plane_ambient_color,
        plane_specular_color,
        256.0  // shininess
    );

    Vec3 sphere_diffuse_color = {{0.8, 0.2, 0.3}};
    Vec3 sphere_ambient_color = sphere_diffuse_color;
    Vec3 sphere_specular_color = {{1.0, 1.0, 1.0}};
    Material* sphere_material = material_create(
        sphere_diffuse_color,
        sphere_ambient_color,
        sphere_specular_color,
        256.0  // shininess
    );

    Vec3 plane_scale = {{1000.0, 1000.0, 1.0}};
    Vec3 plane_rotation = {{-PI / 2, 0.0, 0.0}};
    Vec3 plane_translation = {{0.0, -1.0, 0.0}};
    Transformation plane_transformation = transformation_create(
        plane_scale, plane_rotation, plane_translation);
    mesh_create_plane(&meshes[n_meshes - 1], plane_material, plane_transformation);
    for (size_t i = 0; i < n_meshes - 1; ++i) {
        Vec3 sphere_scale = {{1.0, 1.0, 1.0}};
        Vec3 sphere_rotation = {{0.0, 0.0, 0.0}};
        Vec3 sphere_translation = {{i * 3.0, 0.0, -5.0}};
        Transformation sphere_transformation = transformation_create(
            sphere_scale, sphere_rotation, sphere_translation);

        mesh_create_icosahedron(&meshes[i], sphere_material, sphere_transformation);
    }
}


int main(void) {
    Window window;
    create_window(&window);

    Camera* camera = create_camera();
    window_attach_camera(&window, camera);


    create_point_lights(4);

    Renderer renderer;
    create_renderer(&renderer);

    size_t n_meshes = 5;
    Mesh meshes[n_meshes];
    create_meshes(meshes, n_meshes);

    bool ok = true;
    while (!glfwWindowShouldClose(window.glfw_window)) {
        renderer_set_scene(&renderer, meshes, n_meshes);
        ok &= renderer_draw_scene(&renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (!ok) {
            return 1;
        }
        for (size_t i = 0; i < n_meshes; ++i) {
            if (i != n_meshes - 1) {
                mesh_rotate(&meshes[i], 0.01, 0.01, 0.01);
            }
        }

        glfwSwapBuffers(window.glfw_window);
        glfwPollEvents();
    }

    glfwTerminate();
}

