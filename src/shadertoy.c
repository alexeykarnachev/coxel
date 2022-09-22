#include "includes.h"


static size_t SCR_WIDTH = 800;
static size_t SCR_HEIGHT = 600;

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
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

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shader Toy", NULL, NULL);
    if (window == NULL) {
        printf("ERROR: failed to create GLFW window");
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowAttrib(window, GLFW_FLOATING, GLFW_TRUE);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("ERROR: failed to initialize GLAD");
        exit(-1);
    }

    return window;
}

bool is_file_modified(size_t n_files, const char* file_paths[], time_t* mod_time) {
    time_t file_mod_time = 0.0;
    for (size_t i = 0; i < n_files; ++i) {
        struct stat file_stat;
        int err = stat(file_paths[i], &file_stat);
        if (err != 0) {
            perror("");
            return false;
        }
        file_mod_time = file_mod_time > file_stat.st_mtime ? file_mod_time : file_stat.st_mtime;
    }

    if (file_mod_time > *mod_time || *mod_time == 0) {
        *mod_time = file_mod_time;
        printf("INFO: Last modification time stamp: %ld\n", *mod_time);
        return true;
    }
    return false;
}

float get_time_passed(struct timeval* start_time) {
    struct timeval curr_time;
    gettimeofday(&curr_time, NULL);
    float time = (float)(curr_time.tv_usec - start_time->tv_usec) / 1000000 + (float)(curr_time.tv_sec - start_time->tv_sec);
    return time;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: Please, provide the fragment shader source file as the first argument\n");
        exit(1);
    }

    const char* vert_file_path = "./shaders/common/plane.vert";
    const char* frag_file_path = argv[1];
    const char* globals_file_path = "./shaders/shadertoy/globals.glsl";
    const char* random_file_path = "./shaders/common/random.glsl";
    const char* deps_file_paths[2] = {globals_file_path, random_file_path};
    const char* file_paths[4] = {vert_file_path, frag_file_path, globals_file_path, random_file_path};
    printf("INFO: Rendering fragment shader: %s\n", frag_file_path);

    GLFWwindow *window = create_window();

    glEnable(GL_DEPTH_TEST);
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    GLuint program;

    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    time_t mod_time = 0;
    float time;
    while (!glfwWindowShouldClose(window)) {
        if (is_file_modified(4, file_paths, &mod_time)) {
            glDeleteProgram(program);
            program = glCreateProgram();
            shader_link_program(
                vert_file_path,
                NULL,
                NULL,
                frag_file_path,
                program,
                2,
                deps_file_paths
            );
            glUseProgram(program);
            printf("INFO: Shader program has been reloaded\n");
        }
        time = get_time_passed(&start_time);

        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniform2f(glGetUniformLocation(program, "u_resolution"), SCR_WIDTH, SCR_HEIGHT);
        glUniform1f(glGetUniformLocation(program, "u_time"), time);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

