#include "includes.h"


static unsigned int SCR_WIDTH = 1920;
static unsigned int SCR_HEIGHT = 1080;
static float SIDE_TRANSLATION_SENS = 5.0;
static float FORWARD_TRANSLATION_SENS = 0.5;
static float ROTATION_SENS = 1.0;

static float CURSOR_X;
static float CURSOR_Y;
static bool MMB_PRESSED;
static bool LMB_PRESSED;

static Mat4 MV;
static Mat4 PROJ;
static float TESS_LVL_INNER = 6.0;
static float TESS_LVL_OUTER = 6.0;
static Camera CAMERA;

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    CAMERA.aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    glViewport(0, 0, width, height);
}

static void cam_update() {
    MV = cam_get_view_mat(&CAMERA);
    PROJ = cam_get_perspective_projection_mat(&CAMERA);
}

static void cursor_position_callback(GLFWwindow* window, double x, double y) {
    float xd = (CURSOR_X - x) / SCR_WIDTH;
    float yd = (y - CURSOR_Y) / SCR_HEIGHT;
    CURSOR_X = x;
    CURSOR_Y = y;
    
    if (MMB_PRESSED) {
        cam_translate(&CAMERA, SIDE_TRANSLATION_SENS * xd, SIDE_TRANSLATION_SENS * yd, 0.0f);
        cam_update();
    } 

    if (LMB_PRESSED) {
        cam_rotate(&CAMERA, ROTATION_SENS * yd, -ROTATION_SENS * xd);
        cam_update();
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        MMB_PRESSED = action == GLFW_PRESS;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        LMB_PRESSED = action == GLFW_PRESS;
    }
}

void scroll_callback(GLFWwindow* window, double x, double y) {
    cam_translate(&CAMERA, 0.0f, 0.0f, FORWARD_TRANSLATION_SENS * y);
    cam_update();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

GLFWwindow* create_window() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Coxel", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL) {
        printf("ERROR: failed to create GLFW window");
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("ERROR: failed to initialize GLAD");
        exit(-1);
    }

    return window;
}

int main(void) {
    cam_create(&CAMERA);
    cam_update();
    GLFWwindow *window = create_window();
    glEnable(GL_CULL_FACE);

    Sphere sphere;
    if (!sphere_create(&sphere)) {
        printf("ERROR: failed to create sphere\n");
        glfwTerminate();
        exit(-1);
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sphere_translate(&sphere, 0.01f, 0.01f, 0.01f);
        sphere_rotate(&sphere, 0.01f, 0.01f, 0.01f);
        sphere_draw(&sphere, &CAMERA);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

