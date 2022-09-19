#include "includes.h"


static size_t SCR_WIDTH = 1920;
static size_t SCR_HEIGHT = 1080;
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

    Planet planet;
    Sun sun;
    Bloom bloom;

    int bloom_n_iters = 10;
    Vec3 space_color = {{ 0.0001, 0.0, 0.0001 }};
    Vec3 planet_diffuse_color = {{ 0.9, 0.3, 0.3 }};
    Vec3 sun_color = {{0.9569, 0.9137, 0.6078}};
    sun_color = vec3_scale(&sun_color, 1000.0);

    bool ok = true;
    ok &= sphere_create_sun(&sun);
    ok &= sphere_create_planet(&planet);
    ok &= bloom_create(&bloom, SCR_WIDTH, SCR_HEIGHT);
    if (!ok) {
        printf("ERROR: failed to create scene objects\n");
        glfwTerminate();
        exit(-1);
    }

    sphere_translate(&planet.sphere, -3.0f, -3.0f, -20.0f);
    sphere_translate(&sun.sphere, 3.0f, 3.0f, -20.0f);
    sphere_scale(&sun.sphere, 2.0f, 2.0f, 2.0f);


    glEnable(GL_DEPTH_TEST);
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(space_color.data[0], space_color.data[1], space_color.data[2], 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bloom_bind(&bloom);

        sphere_translate(&planet.sphere, 0.05f, 0.05f, 0.00f);

        sphere_draw_planet(
            &planet,
            &CAMERA,
            &sun.sphere.translation,
            &planet_diffuse_color,
            &space_color,
            &sun_color
        );

        sphere_draw_sun(
            &sun,
            &CAMERA,
            &sun_color
        );

        bloom_draw(&bloom, bloom_n_iters);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

