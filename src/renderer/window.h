typedef struct Window {
    size_t width;
    size_t height;
    float cursor_x;
    float cursor_y;
    bool mmb_pressed;
    bool lmb_pressed;
    Camera* camera;
    GLFWwindow* glfw_window;
} Window;

bool window_create(Window* window, size_t width, size_t height, Camera* camera);
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void scroll_callback(GLFWwindow* window, double x, double y);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void cursor_position_callback(GLFWwindow* window, double x, double y);


bool window_create(Window* window, size_t width, size_t height, Camera* camera) {
    clear_struct(window);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* glfw_window = glfwCreateWindow(width, height, "Coxel", glfwGetPrimaryMonitor(), NULL);
    if (glfw_window == NULL) {
        printf("ERROR: failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    window->width = width;
    window->height = height;
    window->cursor_x = 0.0;
    window->cursor_y = 0.0;
    window->mmb_pressed = false;
    window->lmb_pressed = false;
    window->camera = camera;
    window->glfw_window = glfw_window;

    glfwMakeContextCurrent(glfw_window);
    glfwSetWindowUserPointer(glfw_window, window);
    glfwSetFramebufferSizeCallback(glfw_window, framebuffer_size_callback);
    glfwSetCursorPosCallback(glfw_window, cursor_position_callback);
    glfwSetMouseButtonCallback(glfw_window, mouse_button_callback);
    glfwSetScrollCallback(glfw_window, scroll_callback);
    glfwSetKeyCallback(glfw_window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("ERROR: failed to initialize GLAD");
        glfwTerminate();
        return false;
    }

    return true;
}


static void cursor_position_callback(GLFWwindow* window, double x, double y) {
    Window* w = (Window*)(glfwGetWindowUserPointer(window));
    float xd = (w->cursor_x - x) / w->width;
    float yd = (y - w->cursor_y) / w->height;
    w->cursor_x = x;
    w->cursor_y = y;
    
    if (w->mmb_pressed) {
        camera_translate(w->camera, xd, yd, 0.0);
    } 

    if (w->lmb_pressed) {
        camera_rotate(w->camera, yd, -xd);
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Window* w = (Window*)(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        w->mmb_pressed = action == GLFW_PRESS;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        w->lmb_pressed = action == GLFW_PRESS;
    }
}

static void scroll_callback(GLFWwindow* window, double x, double y) {
    Window* w = (Window*)(glfwGetWindowUserPointer(window));
    camera_translate(w->camera, 0.0f, 0.0f, y);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Window* w = (Window*)(glfwGetWindowUserPointer(window));
    camera_set_aspect(w->camera, (float)width / (float)height);
    w->width = width;
    w->height = height;
    glViewport(0, 0, width, height);
}

