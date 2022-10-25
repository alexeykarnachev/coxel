#include "../includes.h"

size_t SCREEN_WIDTH = 1920;
size_t SCREEN_HEIGHT = 1080;

float CAMERA_FOV = 45.0;
float CAMERA_NEAR_PLANE = 0.1;
float CAMERA_FAR_PLANE = 1000.0;


int main(void) {
    window_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    program_create_all();

    int camera = camera_create(
        CAMERA_FOV,
        CAMERA_NEAR_PLANE,
        CAMERA_FAR_PLANE,
        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
        vec3_zeros()
    );

    CameraController camera_controller = {&CAMERA_ARENA[camera], 0.01, 0.8, 0.001};

    int mesh_0 = mesh_create_icosahedron();
    int transformation_0 = transformation_create(vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -5.0));
    int material_0 = material_create(vec3(0.8, 0.2, 0.3), vec3(0.8, 0.2, 0.3), vec3(1.0, 1.0, 1.0), 256.0);
    int point_light_0 = point_light_create(vec3(0.0, 5.0, -5.0), vec3(1.0, 1.0, 1.0), 200.0);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    while (!INPUT.window_should_close) {
        renderer_update();
        camera_controller_update(&camera_controller);

        glUseProgram(PROGRAM_MATERIAL);
        mesh_bind(mesh_0);
        program_set_attribute(PROGRAM_MATERIAL, "model_pos", 3, GL_FLOAT);
        program_set_uniform_1i(PROGRAM_MATERIAL, "camera_id", 0);
        program_set_uniform_1i(PROGRAM_MATERIAL, "transformation_id", 0);
        program_set_uniform_1i(PROGRAM_MATERIAL, "material_id", 0);
        mesh_draw();

        camera_update_ubo(camera);
        window_update();
    }

    glfwTerminate();
}

