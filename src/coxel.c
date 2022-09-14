#include "includes.h"


static unsigned int SCR_WIDTH = 1920;
static unsigned int SCR_HEIGHT = 1080;

static float CURSOR_X;
static float CURSOR_Y;
static bool MMB_PRESSED;
static bool LMB_PRESSED;

static Mat4 VIEW;
static Mat4 PROJ;

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

static void cam_update() {
    VIEW = cam_get_view();
    PROJ = cam_get_perspective_projection(SCR_WIDTH, SCR_HEIGHT);
}

static void cursor_position_callback(GLFWwindow* window, double x, double y) {
    float xd = (CURSOR_X - x) / SCR_WIDTH;
    float yd = (y - CURSOR_Y) / SCR_HEIGHT;
    CURSOR_X = x;
    CURSOR_Y = y;
    
    if (MMB_PRESSED) {
        cam_move_side(xd, yd);
        cam_update();
    } 

    if (LMB_PRESSED) {
        cam_rotate(yd, -xd);
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
    cam_move_forward(-y);
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
    cam_update();
    GLFWwindow *window = create_window();
    glEnable(GL_CULL_FACE);

    Sphere sphere;
    if (!create_sphere(&sphere)) {
        printf("ERROR: failed to create sphere\n");
        glfwTerminate();
        exit(-1);
    }

    glPatchParameteri(GL_PATCH_VERTICES, 3);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(sphere.program);
        glUniformMatrix4fv(sphere.u_mv_loc, 1, GL_TRUE, (float*)&VIEW);
        glUniformMatrix4fv(sphere.u_proj_loc, 1, GL_TRUE, (float*)&PROJ);
        glUniform1f(sphere.u_tess_lvl_inner_loc, 6.0f);
        glUniform1f(sphere.u_tess_lvl_outer_loc, 6.0f);
        glUniform3f(sphere.u_center_pos_loc, 0.0, 0.0, 0.0);
        glUniform3f(sphere.u_light_pos_loc, 0.0, 5.0, 0.0);

        glBindVertexArray(sphere.vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.ebo);
        glDrawElements(GL_PATCHES, sizeof(ICOSAHEDRON_FACES) / sizeof(ICOSAHEDRON_FACES[0]),  GL_UNSIGNED_BYTE, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

