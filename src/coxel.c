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

GLFWwindow *create_window() {
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
    GLFWwindow *window = create_window();
    cam_update();

    GLuint program = glCreateProgram();
    bool is_linked = link_program_files(
        "./shaders/shader.vert",
        NULL,// "./shaders/shader.tesc",
        NULL,// "./shaders/shader.tese",
        "./shaders/shader.frag",
        program
    );
    if (!is_linked) {
        fprintf(stderr, "ERROR: failed to link program files\n");
        return 1;
    }

    GLint vs_in_pos = glGetAttribLocation(program, "vs_in_pos");
    GLint u_mv = glGetUniformLocation(program, "u_mv");
    GLint u_proj = glGetUniformLocation(program, "u_proj");

    GLuint icos_vbo;
    glCreateBuffers(1, &icos_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, icos_vbo);
    glBufferData(GL_ARRAY_BUFFER, ICOSAHEDRON_N_VERTS_BYTES, ICOSAHEDRON_VERTS, GL_STATIC_DRAW);

    GLuint icos_vao;
    glCreateVertexArrays(1, &icos_vao);
    glBindVertexArray(icos_vao);
    glEnableVertexAttribArray(vs_in_pos);
    glVertexAttribPointer(vs_in_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint icos_ebo;
    glCreateBuffers(1, &icos_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, icos_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ICOSAHEDRON_N_FACES_BYTES, ICOSAHEDRON_FACES, GL_STATIC_DRAW);

    glUseProgram(program);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(4.0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(u_mv, 1, GL_TRUE, (float*)&VIEW);
        glUniformMatrix4fv(u_proj, 1, GL_TRUE, (float*)&PROJ);

        glBindVertexArray(icos_vao);
        glDrawElements(GL_TRIANGLES, ICOSAHEDRON_N_FACES * 3, GL_UNSIGNED_BYTE, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

