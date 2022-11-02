typedef struct Input {
    float t;
    float dt;

    float window_width;
    float window_height;
    bool window_should_close;

    float cursor_x;
    float cursor_y;
    float cursor_dx;
    float cursor_dy;

    float scroll_dy;

    bool mouse_middle_pressed;
    bool mouse_left_pressed;
} Input;

Input INPUT;

void input_clear() {
    INPUT.cursor_dx = 0.0;
    INPUT.cursor_dy = 0.0;
    INPUT.scroll_dy = 0.0;
}

static void cursor_position_callback(GLFWwindow* window, double x, double y) {
    Input* inp = (Input*)(glfwGetWindowUserPointer(window));
    x /= INPUT.window_width;
    y /= INPUT.window_height;
    y = 1.0 - y;

    inp->cursor_dx = inp->cursor_x - x; 
    inp->cursor_dy = inp->cursor_y - y;
    inp->cursor_x = x;
    inp->cursor_y = y;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Input* inp = (Input*)(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        inp->mouse_middle_pressed = action == GLFW_PRESS;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        inp->mouse_left_pressed = action == GLFW_PRESS;
    }
}

static void scroll_callback(GLFWwindow* window, double x, double y) {
    Input* inp = (Input*)(glfwGetWindowUserPointer(window));
    inp->scroll_dy = y;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Input* inp = (Input*)(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        inp->window_should_close = true;
    }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Input* inp = (Input*)(glfwGetWindowUserPointer(window));
    inp->window_width = width;
    inp->window_height = height;
}

GLFWwindow* _WINDOW;

bool window_create(size_t width, size_t height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _WINDOW = glfwCreateWindow(width, height, "Coxel", glfwGetPrimaryMonitor(), NULL);
    if (_WINDOW == NULL) {
        printf("ERROR: failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(_WINDOW);
    glfwSetWindowUserPointer(_WINDOW, &INPUT);

    glfwSetFramebufferSizeCallback(_WINDOW, framebuffer_size_callback);
    glfwSetCursorPosCallback(_WINDOW, cursor_position_callback);
    glfwSetMouseButtonCallback(_WINDOW, mouse_button_callback);
    glfwSetScrollCallback(_WINDOW, scroll_callback);
    glfwSetKeyCallback(_WINDOW, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("ERROR: failed to initialize GLAD");
        glfwTerminate();
        return false;
    }

    return true;
}

void window_update() {
    input_clear();
    glfwSwapBuffers(_WINDOW);
    glfwPollEvents();
}

