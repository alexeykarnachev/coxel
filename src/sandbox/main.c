#include "../includes.h"

size_t SCREEN_WIDTH = 1920;
size_t SCREEN_HEIGHT = 1080;

float CAMERA_FOV = 45.0;
float CAMERA_NEAR_PLANE = 0.1;
float CAMERA_FAR_PLANE = 1000.0;

float CAMERA_SIDE_SENS = 10.0;
float CAMERA_STRAIGHT_SENS = 1.0;
float CAMERA_ROTATION_SENS = 2.0;


// int main(void) {
//     GUIFont gui_font;
//     bool ok = gui_font_create_from_file(&gui_font, "./assets/fonts/Source_Code_Pro/static/SourceCodePro-Regular.fnt", "./assets/fonts/Source_Code_Pro/static/SourceCodePro-Regular.png");
//     return 0;
// }

int main(void) {
    window_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    renderer_create();
    scene_create();

    Camera camera = camera_create(
        CAMERA_FOV,
        CAMERA_NEAR_PLANE,
        CAMERA_FAR_PLANE,
        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT
    );

    Mesh mesh_0 = mesh_create_icosahedron();
    Mesh mesh_1 = mesh_create_plane();
    Material material_0 = material_create(
        vec3(0.8, 0.2, 0.3), vec3(0.8, 0.2, 0.3), vec3(1.0, 1.0, 1.0), 256.0);
    Material material_1 = material_create(
        vec3(0.8, 0.8, 0.1), vec3(0.8, 0.8, 0.1), vec3(1.0, 1.0, 1.0), 256.0);
    Transformation transformation_0 = transformation_create(
        vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -5.0));
    Transformation transformation_1 = transformation_create(
        vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(4.0, 0.0, -5.0));
    Transformation transformation_2 = transformation_create(
        vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(8.0, 0.0, -5.0));
    Transformation transformation_3 = transformation_create(
        vec3(1000.0, 1000.0, 1000.0), vec3(-deg2rad(90.0), 0.0, 0.0), vec3(0.0, -5.0, 0.0));
    PointLight point_light_0 = point_light_create(
        vec3(0.0, 5.0, -5.0), vec3(0.2, 0.8, 0.3), 200.0);
    PointLight point_light_1 = point_light_create(
        vec3(10.0, 5.0, -5.0), vec3(0.2, 0.3, 0.8), 150.0);
    PointShadowCaster point_shadow_caster_0 = point_shadow_caster_create_default(
        vec3(0.0, 15.0, -5.0));
    PointShadowCaster point_shadow_caster_1 = point_shadow_caster_create_default(
        vec3(10.0, 15.0, -5.0));

    GUIPane gui_pane_0 = gui_pane_create(0.1, 0.1, 0.1, 0.8);
    GUIPane gui_pane_1 = gui_pane_create(0.8, 0.1, 0.1, 0.8);

    int32_t camera_gid = scene_add_camera(&camera);
    scene_set_active_camera_gid(camera_gid);
    scene_add_point_light(&point_light_0);
    scene_add_point_light(&point_light_1);
    scene_add_point_shadow_caster(&point_shadow_caster_0);
    scene_add_point_shadow_caster(&point_shadow_caster_1);

    scene_add_mesh(&mesh_0);
    scene_add_material(&material_0);
    scene_add_transformation(&transformation_0);
    scene_add_transformation(&transformation_1);
    scene_add_material(&material_1);
    scene_add_transformation(&transformation_2);

    scene_add_mesh(&mesh_1);
    scene_add_material(&material_1);
    scene_add_transformation(&transformation_3);

    CameraMouseControllerArgs camera_mouse_controller_args = {
        camera_gid, CAMERA_SIDE_SENS, CAMERA_STRAIGHT_SENS, CAMERA_ROTATION_SENS};
    Script camera_mouse_controller_script = camera_mouse_controller_create_script(&camera_mouse_controller_args);
    scene_add_script(&camera_mouse_controller_script);

    scene_add_gui_pane(&gui_pane_0);
    scene_add_gui_pane(&gui_pane_1);

    while (!INPUT.window_should_close) {
        renderer_update();
        window_update();
    }

    glfwTerminate();
}

