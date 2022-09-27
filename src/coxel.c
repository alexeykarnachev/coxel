#include "includes.h"


static size_t SCR_WIDTH = 1920;
static size_t SCR_HEIGHT = 1080;
static float SIDE_TRANSLATION_SENS = 10.0;
static float FORWARD_TRANSLATION_SENS = 1.0;
static float ROTATION_SENS = 1.0;

static float CURSOR_X;
static float CURSOR_Y;
static bool MMB_PRESSED;
static bool LMB_PRESSED;

static Mat4 MV;
static Mat4 PROJ;
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
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    Sphere planet;
    Sphere sun;
    PointShadows point_shadows;
    HDR hdr;

    if (
        !sphere_create(&sun)
        || !sphere_create(&planet)
        || !point_shadows_create(&point_shadows, SCR_WIDTH, SCR_HEIGHT)
        || !hdr_create(&hdr, SCR_WIDTH, SCR_HEIGHT)
    ) {
        exit(-1);
        glfwTerminate();
    }

    Vec3 space_color = {{ 0.0, 0.0, 0.0 }};
    Vec3 planet_diffuse_color = {{ 0.3, 0.4, 1.0 }};
    Vec3 sun_color = {{1.0, 1.0, 1.0}};
    sun_color = vec3_scale(&sun_color, 100000.0);

    sphere_translate(&sun, -50.0f, 50.0f, -500.0f);
    sphere_translate(&planet, 2.0f, -2.0f, -4.0f);
    sphere_scale(&sun, 2.0f, 2.0f, 2.0f);
    sphere_scale(&planet, 1.0f, 1.0f, 1.0f);

    glPatchParameteri(GL_PATCH_VERTICES, 3);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(space_color.data[0], space_color.data[1], space_color.data[2], 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // hdr_bind(&hdr);
        // hdr_draw(&hdr, 0);

        sphere_rotate(&planet, 0.0, 0.002, 0.0);

        point_shadows_bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        sphere_draw(
            &planet,
            &CAMERA,
            64.0,  // tess_lvl_inner
            64.0,  // tess_lvl_outer
            6,     // surface_noise_n_levels
            2.0,   // surface_noise_freq_mult
            0.6,   // surface_noise_ampl_mult
            2.0,   // surface_noise_freq_init
            0.3,   // surface_noise_mult
            &sun.translation,
            &planet_diffuse_color,
            &sun_color,
            0.00001,  // ambient_weight
            128.0   // specular_power
        );

        sphere_draw(
            &sun,
            &CAMERA,
            6.0,  // tess_lvl_inner
            6.0,  // tess_lvl_outer
            0,     // surface_noise_n_levels
            0.0,   // surface_noise_freq_mult
            0.0,   // surface_noise_ampl_mult
            0.0,   // surface_noise_freq_init
            0.0,   // surface_noise_mult
            &sun.translation,
            &sun_color,
            &sun_color,
            1.0,  // ambient_weight
            1.0   // specular_power
        );

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_CUBE_MAP, point_shadows->depth_cubemap);
        sphere_draw(
            &planet,
            &CAMERA,
            64.0,  // tess_lvl_inner
            64.0,  // tess_lvl_outer
            6,     // surface_noise_n_levels
            2.0,   // surface_noise_freq_mult
            0.6,   // surface_noise_ampl_mult
            2.0,   // surface_noise_freq_init
            0.3,   // surface_noise_mult
            &sun.translation,
            &planet_diffuse_color,
            &sun_color,
            0.00001,  // ambient_weight
            128.0   // specular_power
        );

        sphere_draw(
            &sun,
            &CAMERA,
            6.0,  // tess_lvl_inner
            6.0,  // tess_lvl_outer
            0,     // surface_noise_n_levels
            0.0,   // surface_noise_freq_mult
            0.0,   // surface_noise_ampl_mult
            0.0,   // surface_noise_freq_init
            0.0,   // surface_noise_mult
            &sun.translation,
            &sun_color,
            &sun_color,
            1.0,  // ambient_weight
            1.0   // specular_power
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

