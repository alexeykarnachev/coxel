#include "../includes.h"

float CAMERA_FOV = 45.0;
float CAMERA_NEAR_PLANE = 0.1;
float CAMERA_FAR_PLANE = 1000.0;

float CAMERA_SIDE_SENS = 10.0;
float CAMERA_STRAIGHT_SENS = 1.0;
float CAMERA_ROTATION_SENS = 2.0;


int main(void) {
    window_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    renderer_create();
    scene_create();

    ArrayBuffer icosahedron_buffer;
    ArrayBuffer plane_buffer;
    array_buffer_create_icosahedron(&icosahedron_buffer);
    array_buffer_create_plane(&plane_buffer);

    Camera camera = camera_create(
        CAMERA_FOV,
        CAMERA_NEAR_PLANE,
        CAMERA_FAR_PLANE,
        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT
    );
    int32_t camera_id = scene_add_camera(camera);
    SCENE.active_camera_id = camera_id;

    CameraMouseControllerArgs camera_mouse_controller_args = {
        camera_id, CAMERA_SIDE_SENS, CAMERA_STRAIGHT_SENS, CAMERA_ROTATION_SENS};
    Script camera_mouse_controller_script = camera_mouse_controller_create_script(&camera_mouse_controller_args);

    Material red_material = {vec3(0.8, 0.2, 0.2), vec3(0.8, 0.2, 0.2), vec3(1.0, 1.0, 1.0), 64.0};
    Material green_material = {vec3(0.2, 0.8, 0.2), vec3(0.2, 0.8, 0.2), vec3(1.0, 1.0, 1.0), 64.0};
    Material blue_material = {{0.2, 0.2, 0.8}, {0.2, 0.2, 0.8}, {1.0, 1.0, 1.0}, 64.0};
    Material white_material = {{0.9, 0.9, 0.9}, {0.9, 0.9, 0.9}, {1.0, 1.0, 1.0}, 64.0};
    int32_t red_material_id = scene_add_material(red_material);
    int32_t green_material_id = scene_add_material(green_material);
    int32_t blue_material_id = scene_add_material(blue_material);
    int32_t white_material_id = scene_add_material(white_material);

    Mesh icosahedron_mesh_0 = {
        icosahedron_buffer,
        white_material_id,
        {vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -5.0)},
    };

    Mesh icosahedron_mesh_1 = {
        icosahedron_buffer,
        red_material_id,
        {vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(4.0, 0.0, -5.0)},
    };

    Mesh icosahedron_mesh_2 = {
        icosahedron_buffer,
        blue_material_id,
        {vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(-4.0, 0.0, -5.0)},
    };

    Mesh plane_mesh_0 = {
        plane_buffer,
        green_material_id,
        {vec3(1000.0, 1000.0, 1000.0), vec3(-deg2rad(90.0), 0.0, 0.0), vec3(0.0, -5.0, 0.0)}
    };

    PointLight red_point_light = {vec3(0.0, 5.0, -5.0), vec3(0.8, 0.2, 0.2), 50.0};
    PointLight green_point_light = {vec3(4.0, 5.0, -5.0), vec3(0.2, 0.8, 0.2), 50.0};
    PointLight blue_point_light = {vec3(-4.0, 5.0, -5.0), vec3(0.2, 0.2, 0.8), 50.0};

    PointShadowCaster point_shadow_caster_0 = point_shadow_caster_create_default(vec3(0.0, 5.0, -5.0));
    PointShadowCaster point_shadow_caster_1 = point_shadow_caster_create_default(vec3(4.0, 5.0, -5.0));
    PointShadowCaster point_shadow_caster_2 = point_shadow_caster_create_default(vec3(-4.0, 5.0, -5.0));

    GUIPane gui_pane_0 = {0.001, 0.001, 0.18, 0.998};
    GUIText gui_text_0 = gui_text_create("Hello, ZALOOPA!", 13, 0.01, 0.98);

    scene_add_point_light(red_point_light);
    scene_add_point_light(green_point_light);
    scene_add_point_light(blue_point_light);
    scene_add_point_shadow_caster(point_shadow_caster_0);
    scene_add_point_shadow_caster(point_shadow_caster_1);
    scene_add_point_shadow_caster(point_shadow_caster_2);
    scene_add_mesh(icosahedron_mesh_0);
    scene_add_mesh(icosahedron_mesh_1);
    scene_add_mesh(icosahedron_mesh_2);
    scene_add_mesh(plane_mesh_0);
    scene_add_gui_pane(gui_pane_0);
    scene_add_gui_text(gui_text_0);
    scene_add_script(camera_mouse_controller_script);

    while (!INPUT.window_should_close) {
        renderer_update();
        window_update();
    }

    glfwTerminate();
}

