#include "includes.h"


static unsigned int SCR_WIDTH = 800;
static unsigned int SCR_HEIGHT = 600;

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

GLFWwindow *create_window() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Coxel", NULL, NULL);
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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("ERROR: failed to initialize GLAD");
        exit(-1);
    }

    return window;
}

int main(void) {
    GLFWwindow *window = create_window();

    GLuint program = glCreateProgram();
    bool is_linked = link_program_files(
        "./shaders/shader.vert",
        "./shaders/shader.tesc",
        "./shaders/shader.tese",
        "./shaders/shader.frag",
        program
    );
    if (!is_linked) {
        fprintf(stderr, "ERROR: failed to link program files\n");
        return 1;
    }

    glPatchParameteri(GL_PATCH_VERTICES, 4);

    glUseProgram(program);
    GLint vao = 0;
    glCreateVertexArrays(1, &vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.0);

    GLint u_view_loc = glGetUniformLocation(program, "u_view");
    GLint u_proj_loc = glGetUniformLocation(program, "u_proj");

    cam_update();

    while (!glfwWindowShouldClose(window)) {
        glUniformMatrix4fv(u_view_loc, 1, GL_TRUE, (float*)&VIEW);
        glUniformMatrix4fv(u_proj_loc, 1, GL_TRUE, (float*)&PROJ);

        glUseProgram(program);
        glBindVertexArray(vao);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_PATCHES, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

