#include "includes.h"


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void process_input(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
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

    GLint max_patch_vertices = 0;
    glGetIntegerv(GL_MAX_PATCH_VERTICES, &max_patch_vertices);
    printf("GL_MAX_PATCH_VERTICES: %d\n", max_patch_vertices);
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    glUseProgram(program);
    GLint vao = 0;
    glCreateVertexArrays(1, &vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.0);

    GLint u_view_loc = glGetUniformLocation(program, "u_view");
    GLint u_proj_loc = glGetUniformLocation(program, "u_proj");

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        Mat4 u_view = get_view();
        Mat4 u_proj = get_perspective_projection();

        glUniformMatrix4fv(u_view_loc, 1, GL_TRUE, (float*)&u_view);
        glUniformMatrix4fv(u_proj_loc, 1, GL_TRUE, (float*)&u_proj);

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

