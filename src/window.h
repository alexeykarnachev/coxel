typedef struct Input {
    float t;
    float dt;

    float window_width;
    float window_height;
    int window_should_close;

    float cursor_x;
    float cursor_y;
    float cursor_dx;
    float cursor_dy;

    float scroll_dy;

    int mouse_middle_released;
    int mouse_left_released;
    int shift_released;
    int g_released;
    int x_released;
    int y_released;
    int z_released;

    int mouse_middle_pressed;
    int mouse_left_pressed;
    int shift_pressed;
    int g_pressed;
    int x_pressed;
    int y_pressed;
    int z_pressed;
} Input;

Input INPUT;

GLFWwindow* _WINDOW;
static GLFWcursor* _CURSOR_DRAG;
static GLFWcursor* _CURSOR_DEFAULT;


static void cursor_position_callback(GLFWwindow* window, double x, double y) {
    Input* inp = (Input*)(glfwGetWindowUserPointer(window));
    x /= INPUT.window_width;
    y /= INPUT.window_height;
    y = 1.0 - y;

    inp->cursor_dx += x - inp->cursor_x; 
    inp->cursor_dy += y - inp->cursor_y;
    inp->cursor_x = x;
    inp->cursor_y = y;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Input* inp = (Input*)(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        inp->mouse_middle_pressed = action == GLFW_PRESS;
        inp->mouse_middle_released = action == GLFW_RELEASE;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
        inp->mouse_left_pressed = action == GLFW_PRESS;
        inp->mouse_left_released = action == GLFW_RELEASE;
    }
}

static void scroll_callback(GLFWwindow* window, double x, double y) {
    Input* inp = (Input*)(glfwGetWindowUserPointer(window));
    inp->scroll_dy += y;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Input* inp = (Input*)(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        inp->window_should_close = true;
    } else if (key == GLFW_KEY_LEFT_SHIFT) {
        inp->shift_pressed = action == GLFW_PRESS;
        inp->shift_released = action == GLFW_RELEASE;
    } else if (key == GLFW_KEY_G) {
        inp->g_pressed = action == GLFW_PRESS;
        inp->g_released = action == GLFW_RELEASE;
    } else if (key == GLFW_KEY_X) {
        inp->x_pressed = action == GLFW_PRESS;
        inp->x_released = action == GLFW_RELEASE;
    } else if (key == GLFW_KEY_Y) {
        inp->y_pressed = action == GLFW_PRESS;
        inp->y_released = action == GLFW_RELEASE;
    } else if (key == GLFW_KEY_Z) {
        inp->z_pressed = action == GLFW_PRESS;
        inp->z_released = action == GLFW_RELEASE;
    }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Input* inp = (Input*)(glfwGetWindowUserPointer(window));
    inp->window_width = width;
    inp->window_height = height;
}

bool window_create(size_t width, size_t height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // _WINDOW = glfwCreateWindow(width, height, "Coxel", glfwGetPrimaryMonitor(), NULL);
    _WINDOW = glfwCreateWindow(width, height, "Coxel", NULL, NULL);
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
    glfwSetKeyCallback(_WINDOW, key_callback);
    glfwSetScrollCallback(_WINDOW, scroll_callback);
    glfwSetKeyCallback(_WINDOW, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("ERROR: failed to initialize GLAD");
        glfwTerminate();
        return false;
    }

    _CURSOR_DRAG = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    _CURSOR_DEFAULT = glfwCreateStandardCursor(GLFW_ARROW_CURSOR); 

    return true;
}

void window_set_drag_cursor() {
    glfwSetCursor(_WINDOW, _CURSOR_DRAG);
}

void window_set_default_cursor() {
    glfwSetCursor(_WINDOW, _CURSOR_DEFAULT);
}

void window_clear_input() {
    INPUT.cursor_dx = 0.0;
    INPUT.cursor_dy = 0.0;
    INPUT.scroll_dy = 0.0;
    INPUT.mouse_middle_released = 0;
    INPUT.mouse_left_released = 0;
    INPUT.shift_released = 0;
    INPUT.g_released = 0;
    INPUT.x_released = 0;
    INPUT.y_released = 0;
    INPUT.z_released = 0;
}

void window_update() {
    window_clear_input();
    glfwSwapBuffers(_WINDOW);
    glfwPollEvents();
}

