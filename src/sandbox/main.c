#include "../includes.h"

size_t SCREEN_WIDTH = 1920;
size_t SCREEN_HEIGHT = 1080;

float CAMERA_FOV = 45.0;
float CAMERA_NEAR_PLANE = 0.1;
float CAMERA_FAR_PLANE = 1000.0;
float CAMERA_SIDE_SENS = 10.0;
float CAMERA_STRAIGHT_SENS = 10.0;
float CAMERA_ROTATION_SENS = 10.0;


int main(void) {
    Window window;
    window_create(&window, SCREEN_WIDTH, SCREEN_HEIGHT);
    program_create_all();

    int camera = camera_create(
        CAMERA_FOV,
        CAMERA_NEAR_PLANE,
        CAMERA_FAR_PLANE,
        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
        CAMERA_SIDE_SENS,
        CAMERA_STRAIGHT_SENS,
        CAMERA_ROTATION_SENS,
        vec3_zeros()
    );
    // TODO decouple camera and window.
    window_attach_camera(&window, &CAMERA_ARENA[camera]);

    int icosahedron = mesh_create_icosahedron();
    int transformation_0 = transformation_create(vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -5.0));
    int material_0 = material_create(vec3(0.8, 0.2, 0.3), vec3(0.8, 0.2, 0.3), vec3(1.0, 1.0, 1.0), 256.0);
    int point_light_0 = point_light_create(vec3(-5.0, 5.0, 5.0), vec3(1.0, 1.0, 1.0), 200.0);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    while (!glfwWindowShouldClose(window.glfw_window)) {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(PROGRAM_MATERIAL);
        mesh_bind(icosahedron);
        program_set_attribute(PROGRAM_MATERIAL, "model_pos", 3, GL_FLOAT);
        program_set_uniform_1i(PROGRAM_MATERIAL, "camera_id", 0);
        program_set_uniform_1i(PROGRAM_MATERIAL, "transformation_id", 0);
        program_set_uniform_1i(PROGRAM_MATERIAL, "material_id", 0);
        mesh_draw();

        glfwSwapBuffers(window.glfw_window);
        glfwPollEvents();
    }

    glfwTerminate();
}

