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

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    CAMERA.aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    glViewport(0, 0, width, height);
}

static void cursor_position_callback(GLFWwindow* window, double x, double y) {
    float xd = (CURSOR_X - x) / SCR_WIDTH;
    float yd = (y - CURSOR_Y) / SCR_HEIGHT;
    CURSOR_X = x;
    CURSOR_Y = y;
    
    if (MMB_PRESSED) {
        camera_translate(&CAMERA, SIDE_TRANSLATION_SENS * xd, SIDE_TRANSLATION_SENS * yd, 0.0f);
    } 

    if (LMB_PRESSED) {
        camera_rotate(&CAMERA, ROTATION_SENS * yd, -ROTATION_SENS * xd);
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
    camera_translate(&CAMERA, 0.0f, 0.0f, FORWARD_TRANSLATION_SENS * y);
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
    GLFWwindow *window = create_window();

    GLuint material_program = glCreateProgram();
    program_create_material(material_program);

    float camera_fov = 45.0;
    float camera_near = 0.1;
    float camera_far = 1000.0;
    float camera_aspect = 1.7777;
    Vec3 camera_start_pos = {{0.0, 0.0, 0.0}};
    camera_create(&CAMERA, camera_fov, camera_near, camera_far, camera_aspect, camera_start_pos);

    PointLight point_light;
    Vec3 point_light_world_pos = {{0.0, 1.0, 3.0}};
    Vec3 point_light_color = {{1.0, 1.0, 1.0}};
    float point_light_energy = 100.0;
    float point_light_radius = 100.0;
    point_light_create(
        &point_light,
        point_light_world_pos,
        point_light_color,
        point_light_energy,
        point_light_radius
    );

    Material sphere_material;
    Vec3 sphere_diffuse_color = {{0.8, 0.2, 0.3}};
    Vec3 sphere_ambient_color = sphere_diffuse_color;
    Vec3 sphere_specular_color = {{1.0, 1.0, 1.0}};
    float sphere_shininess = 256.0;
    material_create(
        &sphere_material,
        sphere_diffuse_color,
        sphere_ambient_color,
        sphere_specular_color,
        sphere_shininess
    );

    Material plane_material;
    Vec3 plane_diffuse_color = {{0.8, 0.8, 0.8}};
    Vec3 plane_ambient_color = plane_diffuse_color;
    Vec3 plane_specular_color = {{1.0, 1.0, 1.0}};
    float plane_shininess = 256.0;
    material_create(
        &plane_material,
        plane_diffuse_color,
        plane_ambient_color,
        plane_specular_color,
        plane_shininess
    );

    Transformation sphere_transformation;
    Vec3 sphere_scale = {{1.0, 1.0, 1.0}};
    Vec3 sphere_rotation = {{0.0, 0.0, 0.0}};
    Vec3 sphere_translation = {{0.0, 0.0, -5.0}};
    transformation_create(&sphere_transformation, sphere_scale, sphere_rotation, sphere_translation);

    Transformation plane_transformation;
    Vec3 plane_scale = {{10.0, 10.0, 10.0}};
    Vec3 plane_rotation = {{0.0, 0.0, 0.0}};
    Vec3 plane_translation = {{0.0, 0.0, -10.0}};
    transformation_create(&plane_transformation, plane_scale, plane_rotation, plane_translation);

    Mesh sphere_mesh;
    mesh_create_icosahedron(&sphere_mesh, &sphere_material, &sphere_transformation);

    Mesh plane_mesh;
    mesh_create_plane(&plane_mesh, &plane_material, &plane_transformation);

    Renderer renderer;
    renderer_create(&renderer, material_program);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer_bind_scene(&renderer, &CAMERA, &point_light);
        renderer_bind_mesh(&renderer, &sphere_mesh);
        renderer_draw_triangles(&renderer);

        glfwSwapBuffers(window);
        glfwPollEvents();

        mesh_rotate(&sphere_mesh, 0.01, 0.01, 0.01);
    }
    glfwTerminate();
}

