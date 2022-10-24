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

    size_t camera = camera_create(
        CAMERA_FOV,
        CAMERA_NEAR_PLANE,
        CAMERA_FAR_PLANE,
        (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
        CAMERA_SIDE_SENS,
        CAMERA_STRAIGHT_SENS,
        CAMERA_ROTATION_SENS,
        vec3_zeros()
    );
    window_attach_camera(&window, &CAMERA_ARENA[camera]);

    while (!glfwWindowShouldClose(window.glfw_window)) {
        glfwSwapBuffers(window.glfw_window);
        glfwPollEvents();
    }

    glfwTerminate();
}

