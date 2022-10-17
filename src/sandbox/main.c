#include "../includes.h"


int main(void) {
    size_t screen_width = 1920;
    size_t screen_height = 1080;

    Camera camera;
    Vec3 camera_start_pos = {{0.0, 0.0, 0.0}};
    camera_create(
        &camera,
        45.0,  // fov
        0.1,  // near
        1000.0,  // far
        screen_width / screen_height,  // aspect
        10.0,  // side_sens
        1.0,  // straight_sens
        1.0,  // rotation_sens
        camera_start_pos
    );

    Window window;
    window_create(&window, screen_width, screen_height, &camera);

    Renderer renderer;
    renderer_create(
        &renderer,
        1024,  // shadow_size
        0.1,  // shadow_min_dist
        1000.0  // shadow_max_dist
    );

    PointLight point_light;
    Vec3 point_light_world_pos = {{0.0, 0.0, 5.0}};
    Vec3 point_light_color = {{1.0, 1.0, 1.0}};
    point_light_create(
        &point_light,
        point_light_world_pos,
        point_light_color,
        200.0  // energy
    );

    size_t n_meshes = 10;
    Mesh meshes[n_meshes];

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

    Vec3 plane_scale = {{1000.0, 10.0, 0.0}};
    Vec3 plane_rotation = {{0.0, 0.0, 0.0}};
    Vec3 plane_translation = {{0.0, 0.0, -10.0}};
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

    while (!glfwWindowShouldClose(window.glfw_window)) {
        renderer_set_scene(&renderer, &camera, &point_light);
        for (size_t i = 0; i < n_meshes; ++i) {
            renderer_draw_shadows(&renderer, &meshes[i]);
            if (i != n_meshes - 1) {
                mesh_rotate(&meshes[i], 0.01, 0.01, 0.01);
            }
        }
        for (size_t i = 0; i < n_meshes; ++i) {
            renderer_draw_materials(&renderer, &meshes[i], screen_width, screen_height);
        }

        glfwSwapBuffers(window.glfw_window);
        glfwPollEvents();

    }
    glfwTerminate();
}

