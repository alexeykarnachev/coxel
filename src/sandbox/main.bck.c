#include "../includes.h"


static size_t SCR_WIDTH = 1920;
static size_t SCR_HEIGHT = 1080;
static size_t SHADOW_WIDTH = 1024;
static size_t SHADOW_HEIGHT = 1024;
static size_t SHADOW_NEAR = 1.0;
static size_t SHADOW_FAR = 20.0;
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

    GLuint sphere_material_program = glCreateProgram();
    GLuint sphere_shadow_program = glCreateProgram();
    GLuint plane_material_program = glCreateProgram();
    const char* deps_file_paths[] = {GLSL_RANDOM};

    gl_link_program(
        sphere_material_program,
        VERT_MODEL_SPACE, TESC_TRIANGLE, TESE_PERLIN_TRIANGLES, NULL, FRAG_BLINN_PHONG, 1, deps_file_paths);

    gl_link_program(
        plane_material_program,
        VERT_PROJ_SPACE, NULL, NULL, NULL, FRAG_BLINN_PHONG, 1, deps_file_paths);

    gl_link_program(
        sphere_shadow_program,
        VERT_MODEL_SPACE, TESC_TRIANGLE, TESE_PERLIN_TRIANGLES, GEOM_POINT_SHADOWS, FRAG_SHADOWS_DEPTH, 1, deps_file_paths);

    GLuint shadow_fbo;
    GLuint shadow_tex;
    gl_create_fbo_with_cube_tex(&shadow_fbo, &shadow_tex, SHADOW_WIDTH, SHADOW_HEIGHT);

    Vec3 light_world_pos = {{ 0.0, 1.0, 4.0 }};
    Mat4 light_proj_mat = get_perspective_projection_mat(
        deg2rad(90.0), SHADOW_NEAR, SHADOW_FAR, SHADOW_WIDTH / SHADOW_HEIGHT);

    Mat4 light_view_mats[6];
    Mat4 light_vp_mats[6];
    light_view_mats[0] = get_view_mat(&vec3_pos_x, &vec3_neg_y, &light_world_pos);
    light_view_mats[1] = get_view_mat(&vec3_neg_x, &vec3_neg_y, &light_world_pos);
    light_view_mats[2] = get_view_mat(&vec3_pos_y, &vec3_pos_z, &light_world_pos);
    light_view_mats[3] = get_view_mat(&vec3_neg_y, &vec3_neg_z, &light_world_pos);
    light_view_mats[4] = get_view_mat(&vec3_pos_z, &vec3_neg_y, &light_world_pos);
    light_view_mats[5] = get_view_mat(&vec3_neg_z, &vec3_neg_y, &light_world_pos);
    for (size_t i = 0; i < 6; ++i) {
        light_vp_mats[i] = mat4_mat4_mul(&light_proj_mat, &light_view_mats[i]);
    }

    Vec3 sphere_center_model_pos = {{ 0.0, 0.0, 0.0 }};
    Vec3 sphere_scale = {{ 1.0, 1.0, 1.0 }};
    Vec3 sphere_rotation = {{ 0.0, 0.0, 0.0 }};
    Vec3 sphere_translation = {{ 0.0, 0.0, -4.0 }};
    Mat4 sphere_world_mat = get_world_mat(&sphere_scale, &sphere_rotation, &sphere_translation);

    Vec3 plane_scale = {{ 10.0, 10.0, 10.0 }};
    Vec3 plane_rotation = {{ 0.0, 0.0, 0.0 }};
    Vec3 plane_translation = {{ 0.0, 0.0, -10.0 }};
    Mat4 plane_world_mat = get_world_mat(&plane_scale, &plane_rotation, &plane_translation);

    Model sphere_model;
    Model plane_model;
    model_create(&sphere_model, ICOSAHEDRON_FACES, ICOSAHEDRON_VERTS);
    model_create(&plane_model, PLANE_FACES, PLANE_VERTS);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    while (!glfwWindowShouldClose(window)) {
        cam_update();

        glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(sphere_shadow_program);
        gl_set_program_attribute(sphere_shadow_program, "model_pos", 3, GL_FLOAT);
        gl_set_program_uniform_matrix_4fv(sphere_shadow_program, "world_mat", sphere_world_mat.data, 1, GL_TRUE);
        gl_set_program_uniform_matrix_4fv(sphere_shadow_program, "light_vp_mats[0]", light_vp_mats[0].data, 1, GL_TRUE);
        gl_set_program_uniform_matrix_4fv(sphere_shadow_program, "light_vp_mats[1]", light_vp_mats[1].data, 1, GL_TRUE);
        gl_set_program_uniform_matrix_4fv(sphere_shadow_program, "light_vp_mats[2]", light_vp_mats[2].data, 1, GL_TRUE);
        gl_set_program_uniform_matrix_4fv(sphere_shadow_program, "light_vp_mats[3]", light_vp_mats[3].data, 1, GL_TRUE);
        gl_set_program_uniform_matrix_4fv(sphere_shadow_program, "light_vp_mats[4]", light_vp_mats[4].data, 1, GL_TRUE);
        gl_set_program_uniform_matrix_4fv(sphere_shadow_program, "light_vp_mats[5]", light_vp_mats[5].data, 1, GL_TRUE);
        gl_set_program_uniform_3fv(sphere_shadow_program, "light_world_pos", light_world_pos.data, 1);
        gl_set_program_uniform_3fv(sphere_shadow_program, "center_model_pos", sphere_center_model_pos.data, 1);
        gl_set_program_uniform_1f(sphere_shadow_program, "far", SHADOW_FAR);
        model_draw_patches(&sphere_model, 3);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_tex);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(sphere_material_program);
        gl_set_program_attribute(sphere_material_program, "model_pos", 3, GL_FLOAT);
        gl_set_program_uniform_matrix_4fv(sphere_material_program, "world_mat", sphere_world_mat.data, 1, GL_TRUE);
        gl_set_program_uniform_matrix_4fv(sphere_material_program, "view_mat", CAM_VIEW_MAT.data, 1, GL_TRUE);
        gl_set_program_uniform_matrix_4fv(sphere_material_program, "proj_mat", CAM_PROJ_MAT.data, 1, GL_TRUE);
        gl_set_program_uniform_3fv(sphere_material_program, "center_model_pos", sphere_center_model_pos.data, 1);
        gl_set_program_uniform_1f(sphere_material_program, "far", SHADOW_FAR);
        gl_set_program_uniform_3fv(sphere_material_program, "light_world_pos", light_world_pos.data, 1);
        gl_set_program_uniform_3fv(sphere_material_program, "eye_world_pos", CAMERA.translation.data, 1);
        gl_set_program_uniform_1i(sphere_material_program, "with_shadows", 1);
        model_draw_patches(&sphere_model, 3);

        glUseProgram(plane_material_program);
        gl_set_program_attribute(plane_material_program, "model_pos", 3, GL_FLOAT);
        gl_set_program_uniform_matrix_4fv(plane_material_program, "world_mat", plane_world_mat.data, 1, GL_TRUE);
        gl_set_program_uniform_matrix_4fv(plane_material_program, "view_mat", CAM_VIEW_MAT.data, 1, GL_TRUE);
        gl_set_program_uniform_matrix_4fv(plane_material_program, "proj_mat", CAM_PROJ_MAT.data, 1, GL_TRUE);
        gl_set_program_uniform_1f(plane_material_program, "far", SHADOW_FAR);
        gl_set_program_uniform_3fv(plane_material_program, "light_world_pos", light_world_pos.data, 1);
        gl_set_program_uniform_3fv(plane_material_program, "eye_world_pos", CAMERA.translation.data, 1);
        gl_set_program_uniform_1i(plane_material_program, "with_shadows", 1);
        model_draw_triangles(&plane_model);

        glfwSwapBuffers(window);
        glfwPollEvents();

        sphere_rotation.data[0] += 0.01;
        sphere_world_mat = get_world_mat(&sphere_scale, &sphere_rotation, &sphere_translation);
    }
    glfwTerminate();
}

