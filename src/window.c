#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "window.h"


Input INPUT;

static GLFWwindow* WINDOW;
static GLFWcursor* CURSOR_DRAG;
static GLFWcursor* CURSOR_DEFAULT;

static void cursor_position_callback(
    GLFWwindow* window,
    double x,
    double y
) {
    Input* inp = (Input*)(glfwGetWindowUserPointer(window));
    x /= INPUT.window_width;
    y /= INPUT.window_height;
    y = 1.0 - y;

    inp->cursor_dx += x - inp->cursor_x; 
    inp->cursor_dy += y - inp->cursor_y;
    inp->cursor_x = x;
    inp->cursor_y = y;
}

static void mouse_button_callback(
    GLFWwindow* window,
    int button,
    int action,
    int mods
) {
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

static void key_callback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
) {
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
    } else if (key == GLFW_KEY_S) {
        inp->s_pressed = action == GLFW_PRESS;
        inp->s_released = action == GLFW_RELEASE;
    } else if (key == GLFW_KEY_R) {
        inp->r_pressed = action == GLFW_PRESS;
        inp->r_released = action == GLFW_RELEASE;
    }

    if (action == GLFW_PRESS) {
        inp->last_pressed_key = key;
    } else if (action == GLFW_RELEASE) {
        inp->last_released_key = key;
    }
}

static void framebuffer_size_callback(
    GLFWwindow* window,
    int width,
    int height
) {
    Input* inp = (Input*)(glfwGetWindowUserPointer(window));
    inp->window_width = width;
    inp->window_height = height;
}

bool window_create(size_t width, size_t height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // WINDOW = glfwCreateWindow(width, height, "Coxel", glfwGetPrimaryMonitor(), NULL);
    WINDOW = glfwCreateWindow(width, height, "Coxel", NULL, NULL);
    if (WINDOW == NULL) {
        printf("ERROR: failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(WINDOW);
    glfwSetWindowUserPointer(WINDOW, &INPUT);

    glfwSetFramebufferSizeCallback(WINDOW, framebuffer_size_callback);
    glfwSetCursorPosCallback(WINDOW, cursor_position_callback);
    glfwSetMouseButtonCallback(WINDOW, mouse_button_callback);
    glfwSetKeyCallback(WINDOW, key_callback);
    glfwSetScrollCallback(WINDOW, scroll_callback);
    glfwSetKeyCallback(WINDOW, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("ERROR: failed to initialize GLAD");
        glfwTerminate();
        return false;
    }

    CURSOR_DRAG = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    CURSOR_DEFAULT = glfwCreateStandardCursor(GLFW_ARROW_CURSOR); 

    return true;
}

void window_set_drag_cursor() {
    glfwSetCursor(WINDOW, CURSOR_DRAG);
}

void window_set_default_cursor() {
    glfwSetCursor(WINDOW, CURSOR_DEFAULT);
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
    glfwSwapBuffers(WINDOW);
    glfwPollEvents();
}

