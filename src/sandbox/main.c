#include "../includes.h"


static size_t SCR_WIDTH = 1920;
static size_t SCR_HEIGHT = 1080;
static size_t SHADOW_WIDTH = 1024;
static size_t SHADOW_HEIGHT = 1024;
static size_t SHADOW_NEAR = 1.0;
static size_t SHADOW_FAR = 25.0;
static float SIDE_TRANSLATION_SENS = 10.0;
static float FORWARD_TRANSLATION_SENS = 1.0;
static float ROTATION_SENS = 1.0;

static float CURSOR_X;
static float CURSOR_Y;
static bool MMB_PRESSED;
static bool LMB_PRESSED;

static Camera CAMERA;
Mat4 CAM_VIEW_MAT;
Mat4 CAM_PROJ_MAT;

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    CAMERA.aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    glViewport(0, 0, width, height);
}

static void cam_update() {
    CAM_VIEW_MAT = cam_get_view_mat(&CAMERA);
    CAM_PROJ_MAT = cam_get_perspective_projection_mat(&CAMERA);
}

static void cursor_position_callback(GLFWwindow* window, double x, double y) {
    float xd = (CURSOR_X - x) / SCR_WIDTH;
    float yd = (y - CURSOR_Y) / SCR_HEIGHT;
    CURSOR_X = x;
    CURSOR_Y = y;
    
    if (MMB_PRESSED) {
        cam_translate(&CAMERA, SIDE_TRANSLATION_SENS * xd, SIDE_TRANSLATION_SENS * yd, 0.0f);
    } 

    if (LMB_PRESSED) {
        cam_rotate(&CAMERA, ROTATION_SENS * yd, -ROTATION_SENS * xd);
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
    GLFWwindow *window = create_window();

    Program sphere_material_program;
    Program plane_material_program;
    Program sphere_shadow_program;
    const char* deps_file_paths[] = {"./shaders/common/random.glsl"};

    program_create(
        &sphere_material_program,
        "./shaders/space/model.vert",
        "./shaders/tessellation/simple_3_verts.tesc",
        "./shaders/tessellation/perlin_triangles.tese",
        NULL,
        "./shaders/lighting/blinn_phong.frag",
        1, deps_file_paths
    );

    program_create(
        &plane_material_program,
        "./shaders/space/proj.vert",
        NULL,
        NULL,
        NULL,
        "./shaders/lighting/blinn_phong.frag",
        1, deps_file_paths
    );

    program_create(
        &sphere_shadow_program,
        "./shaders/space/model.vert",
        "./shaders/tessellation/simple_3_verts.tesc",
        "./shaders/tessellation/perlin_triangles.tese",
        NULL,
        "./shaders/common/empty.frag",
        1, deps_file_paths
    );

    GLuint shadow_fbo;
    GLuint shadow_tex;
    glGenFramebuffers(1, &shadow_fbo);
    glGenTextures(1, &shadow_tex);
    glBindTexture(GL_TEXTURE_2D, shadow_tex);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_tex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "ERROR: shadow framebuffer not complete\n");
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Vec3 light_world_pos = {{ 0.0, 2.0, 2.0 }};
    Mat4 light_view_mat = get_view_mat(&vec3_neg_z, &vec3_pos_y, &light_world_pos);
    Mat4 light_proj_mat = get_perspective_projection_mat(
        90.0, SHADOW_NEAR, SHADOW_FAR, SHADOW_WIDTH / SHADOW_HEIGHT);
    Vec3 sphere_center_model_pos = {{ 0.0, 0.0, 0.0 }};
    Vec3 sphere_scale = {{ 1.0, 1.0, 1.0 }};
    Vec3 sphere_rotation = {{ 0.0, 0.0, 0.0 }};
    Vec3 sphere_translation = {{ 0.0, 0.0, -5.0 }};
    Mat4 sphere_world_mat = get_world_mat(&sphere_scale, &sphere_rotation, &sphere_translation);

    Vec3 plane_scale = {{ 10.0, 10.0, 10.0 }};
    Vec3 plane_rotation = {{ 0.0, 0.0, 0.0 }};
    Vec3 plane_translation = {{ 0.0, 0.0, -7.0 }};
    Mat4 plane_world_mat = get_world_mat(&plane_scale, &plane_rotation, &plane_translation);

    Model sphere_model;
    Model plane_model;
    model_create(
        &sphere_model, ICOSAHEDRON_FACES, sizeof(ICOSAHEDRON_FACES), ICOSAHEDRON_VERTS, sizeof(ICOSAHEDRON_VERTS));
    model_create(&plane_model, PLANE_FACES, sizeof(PLANE_FACES), PLANE_VERTS, sizeof(PLANE_VERTS));

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {
        cam_update();

        glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);

        glUseProgram(sphere_shadow_program.gl_program);
        program_set_attribute(&sphere_shadow_program, "model_pos", 3, GL_FLOAT);
        program_set_uniform_matrix_4fv(&sphere_shadow_program, "world_mat", sphere_world_mat.data, 1, GL_TRUE);
        program_set_uniform_matrix_4fv(&sphere_shadow_program, "view_mat", light_view_mat.data, 1, GL_TRUE);
        program_set_uniform_matrix_4fv(&sphere_shadow_program, "proj_mat", light_proj_mat.data, 1, GL_TRUE);
        program_set_uniform_3fv(&sphere_shadow_program, "center_model_pos", sphere_center_model_pos.data, 1);
        model_draw_patches(&sphere_model, 3);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, shadow_tex);
        glClearColor(0.8, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(sphere_material_program.gl_program);
        program_set_attribute(&sphere_material_program, "model_pos", 3, GL_FLOAT);
        program_set_uniform_matrix_4fv(&sphere_material_program, "world_mat", sphere_world_mat.data, 1, GL_TRUE);
        program_set_uniform_matrix_4fv(&sphere_material_program, "view_mat", CAM_VIEW_MAT.data, 1, GL_TRUE);
        program_set_uniform_matrix_4fv(&sphere_material_program, "proj_mat", CAM_PROJ_MAT.data, 1, GL_TRUE);
        program_set_uniform_matrix_4fv(&sphere_material_program, "light_view_mat", light_view_mat.data, 1, GL_TRUE);
        program_set_uniform_matrix_4fv(&sphere_material_program, "light_proj_mat", light_proj_mat.data, 1, GL_TRUE);
        program_set_uniform_3fv(&sphere_material_program, "center_model_pos", sphere_center_model_pos.data, 1);
        program_set_uniform_3fv(&sphere_material_program, "light_world_pos", light_world_pos.data, 1);
        program_set_uniform_3fv(&sphere_material_program, "eye_world_pos", CAMERA.translation.data, 1);
        program_set_uniform_1i(&sphere_material_program, "with_shadows", 1);
        model_draw_patches(&sphere_model, 3);

        glUseProgram(plane_material_program.gl_program);
        program_set_attribute(&plane_material_program, "model_pos", 3, GL_FLOAT);
        program_set_uniform_matrix_4fv(&plane_material_program, "world_mat", plane_world_mat.data, 1, GL_TRUE);
        program_set_uniform_matrix_4fv(&plane_material_program, "view_mat", CAM_VIEW_MAT.data, 1, GL_TRUE);
        program_set_uniform_matrix_4fv(&plane_material_program, "proj_mat", CAM_PROJ_MAT.data, 1, GL_TRUE);
        program_set_uniform_matrix_4fv(&plane_material_program, "light_view_mat", light_view_mat.data, 1, GL_TRUE);
        program_set_uniform_matrix_4fv(&plane_material_program, "light_proj_mat", light_proj_mat.data, 1, GL_TRUE);
        program_set_uniform_3fv(&plane_material_program, "light_world_pos", light_world_pos.data, 1);
        program_set_uniform_3fv(&plane_material_program, "eye_world_pos", CAMERA.translation.data, 1);
        program_set_uniform_1i(&plane_material_program, "with_shadows", 1);
        model_draw_triangles(&plane_model);

        glfwSwapBuffers(window);
        glfwPollEvents();

        sphere_rotation.data[0] += 0.01;
        sphere_world_mat = get_world_mat(&sphere_scale, &sphere_rotation, &sphere_translation);
    }
    glfwTerminate();
}

