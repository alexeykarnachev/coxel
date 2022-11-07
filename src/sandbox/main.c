#include "../includes.h"

int main(void) {
    window_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    Renderer renderer;
    renderer_create(&renderer, POINT_SHADOW_SIZE);

    size_t plane_0 = ecs_create_entity();
    ecs_add_component(
        plane_0, TRANSFORMATION_T, 
        transformation_create(
            vec3(1000.0, 1000.0, 1000.0),
            vec3(-deg2rad(90.0), 0.0, 0.0),
            vec3(0.0, -5.0, 0.0)
        )
    );
    ecs_add_component(
        plane_0, MESH_T, mesh_create_plane());
    ecs_add_component(
        plane_0, MATERIAL_T, material_create_default());

    size_t sphere_0 = ecs_create_entity();
    ecs_add_component(
        sphere_0, TRANSFORMATION_T, 
        transformation_create(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, 0.0),
            vec3(-2.0, 2.0, -2.0)
        )
    );
    ecs_add_component(
        sphere_0, MESH_T, mesh_create_icosahedron());
    ecs_add_component(
        sphere_0, MATERIAL_T, 
        material_create(
            vec3(0.9, 0.2, 0.2),
            vec3(0.009, 0.002, 0.002),
            vec3(1.0, 1.0, 1.0),
            256
        )
    );
    ecs_add_component(sphere_0, HAS_POINT_SHADOW_T, NULL);

    size_t sphere_1 = ecs_create_entity();
    ecs_add_component(
        sphere_1, TRANSFORMATION_T, 
        transformation_create(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, 0.0),
            vec3(0.0, 2.0, -2.0)
        )
    );
    ecs_add_component(
        sphere_1, MESH_T, mesh_create_icosahedron());
    ecs_add_component(
        sphere_1, MATERIAL_T, 
        material_create(
            vec3(0.2, 0.9, 0.2),
            vec3(0.002, 0.009, 0.002),
            vec3(1.0, 1.0, 1.0),
            256
        )
    );
    ecs_add_component(sphere_1, HAS_POINT_SHADOW_T, NULL);

    size_t sphere_2 = ecs_create_entity();
    ecs_add_component(
        sphere_2, TRANSFORMATION_T, 
        transformation_create(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, 0.0),
            vec3(2.0, 2.0, -2.0)
        )
    );
    ecs_add_component(
        sphere_2, MESH_T, mesh_create_icosahedron());
    ecs_add_component(
        sphere_2, MATERIAL_T, 
        material_create(
            vec3(0.2, 0.2, 0.9),
            vec3(0.002, 0.002, 0.009),
            vec3(1.0, 1.0, 1.0),
            256
        )
    );
    ecs_add_component(sphere_2, HAS_POINT_SHADOW_T, NULL);

    size_t point_light_0 = ecs_create_entity();
    ecs_add_component(
        point_light_0, TRANSFORMATION_T,
        transformation_create(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, 0.0),
            vec3(0.0, 12.0, 0.0)
        )
    );
    ecs_add_component(
        point_light_0, POINT_LIGHT_T, point_light_create_default());
    ecs_add_component(
        point_light_0, POINT_SHADOW_CASTER_T, point_shadow_caster_create_default());

    size_t point_light_1 = ecs_create_entity();
    ecs_add_component(
        point_light_1, TRANSFORMATION_T,
        transformation_create(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, 0.0),
            vec3(0.0, 7.0, 7.0)
        )
    );
    ecs_add_component(
        point_light_1, POINT_LIGHT_T, point_light_create_default());
    ecs_add_component(
        point_light_1, POINT_SHADOW_CASTER_T, point_shadow_caster_create_default());

    size_t camera_0 = ecs_create_entity();
    ecs_add_component(
        camera_0, TRANSFORMATION_T,
        transformation_create(vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 5.0))
    );
    ecs_add_component(
        camera_0, CAMERA_T, camera_create_default());
    CameraMouseControllerArgs camera_mouse_controller_args =
        camera_mouse_controller_create_default_args();
    ecs_add_component(
        camera_0, SCRIPT_T,
        camera_mouse_controller_create_script(
            &camera_mouse_controller_args));

    while (!INPUT.window_should_close) {
        window_update();
        ecs_update();
        renderer_update(&renderer);
    }

    glfwTerminate();
}

int main_(void) {
    // renderer_create();
    // scene_create();

    // VAOBuffer icosahedron_buffer;
    // VAOBuffer plane_buffer;
    // vao_buffer_create_icosahedron(&icosahedron_buffer);
    // vao_buffer_create_plane(&plane_buffer);

    // Camera camera = camera_create(
    //     CAMERA_FOV,
    //     CAMERA_NEAR_PLANE,
    //     CAMERA_FAR_PLANE,
    //     (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT
    // );
    // int32_t camera_id = scene_add_camera(camera);
    // SCENE.active_camera_id = camera_id;

    // CameraMouseControllerArgs camera_mouse_controller_args = {
    //     camera_id, CAMERA_SIDE_SENS, CAMERA_STRAIGHT_SENS, CAMERA_ROTATION_SENS};
    // MeshSelectionArgs mesh_selection_args = {-1, -1, vec3(0.25, 0.25, 0.0), vec3(0.5, 0.5, 0.0)};
    // Script camera_mouse_controller_script = camera_mouse_controller_create_script(&camera_mouse_controller_args);
    // Script mesh_selection_script = mesh_selection_create_script(&mesh_selection_args);

    // Material red_material = {vec3(0.8, 0.2, 0.2), vec3(0.008, 0.002, 0.002), vec3(1.0, 1.0, 1.0), vec3_zeros(), 64.0};
    // Material green_material = {vec3(0.2, 0.8, 0.2), vec3(0.002, 0.008, 0.002), vec3(1.0, 1.0, 1.0), vec3_zeros(), 64.0};
    // Material blue_material = {{0.2, 0.2, 0.8}, {0.002, 0.002, 0.008}, {1.0, 1.0, 1.0}, vec3_zeros(), 64.0};
    // Material white_material = {{0.9, 0.9, 0.9}, {0.009, 0.009, 0.009}, {1.0, 1.0, 1.0}, vec3_zeros(), 64.0};
    // int32_t red_material_id = scene_add_material(red_material);
    // int32_t green_material_id = scene_add_material(green_material);
    // int32_t blue_material_id = scene_add_material(blue_material);
    // int32_t white_material_id = scene_add_material(white_material);

    // Mesh icosahedron_mesh_0 = {
    //     icosahedron_buffer,
    //     white_material_id,
    //     {vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -5.0)},
    // };

    // Mesh icosahedron_mesh_1 = {
    //     icosahedron_buffer,
    //     red_material_id,
    //     {vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(4.0, 0.0, -5.0)},
    // };

    // Mesh icosahedron_mesh_2 = {
    //     icosahedron_buffer,
    //     blue_material_id,
    //     {vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(-4.0, 0.0, -5.0)},
    // };

    // Mesh plane_mesh_0 = {
    //     plane_buffer,
    //     green_material_id,
    //     {vec3(1000.0, 1000.0, 1000.0), vec3(-deg2rad(90.0), 0.0, 0.0), vec3(0.0, -5.0, 0.0)}
    // };

    // PointLight red_point_light = {vec3(0.0, 5.0, -5.0), vec3(0.8, 0.2, 0.2), 50.0};
    // PointLight green_point_light = {vec3(4.0, 5.0, -5.0), vec3(0.2, 0.8, 0.2), 50.0};
    // PointLight blue_point_light = {vec3(-4.0, 5.0, -5.0), vec3(0.2, 0.2, 0.8), 50.0};

    // PointShadowCaster point_shadow_caster_0 = point_shadow_caster_create_default(vec3(0.0, 5.0, -5.0));
    // PointShadowCaster point_shadow_caster_1 = point_shadow_caster_create_default(vec3(4.0, 5.0, -5.0));
    // PointShadowCaster point_shadow_caster_2 = point_shadow_caster_create_default(vec3(-4.0, 5.0, -5.0));

    // size_t pane_header_font_height = 18;
    // float h = (float)pane_header_font_height / (float)SCREEN_HEIGHT;
    // GUIPane gui_pane_0 = {0.001, 0.9, 0.998, 0.099};
    // GUIText gui_text_0 = gui_text_create(
    //     "Hello, ZALOOPA!",
    //     pane_header_font_height,
    //     gui_pane_0.x + 0.2 * h,
    //     gui_pane_0.y + gui_pane_0.height - 1.2 * h
    // );

    // scene_add_point_light(red_point_light);
    // scene_add_point_light(green_point_light);
    // scene_add_point_light(blue_point_light);
    // scene_add_point_shadow_caster(point_shadow_caster_0);
    // scene_add_point_shadow_caster(point_shadow_caster_1);
    // scene_add_point_shadow_caster(point_shadow_caster_2);
    // scene_add_mesh(icosahedron_mesh_0);
    // scene_add_mesh(icosahedron_mesh_1);
    // scene_add_mesh(icosahedron_mesh_2);
    // scene_add_mesh(plane_mesh_0);
    // scene_add_gui_pane(gui_pane_0);
    // scene_add_gui_text(gui_text_0);
    // scene_add_script(camera_mouse_controller_script);
    // scene_add_script(mesh_selection_script);

    // while (!INPUT.window_should_close) {
    //     renderer_update();
    //     window_update();
    // }

    // glfwTerminate();
}

