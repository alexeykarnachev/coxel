#pragma once
#include <stdbool.h>
#include <stddef.h>

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
    int s_released;
    int r_released;

    int mouse_middle_pressed;
    int mouse_left_pressed;
    int shift_pressed;
    int g_pressed;
    int x_pressed;
    int y_pressed;
    int z_pressed;
    int s_pressed;
    int r_pressed;

    int last_pressed_key;
    int last_released_key;
} Input;

extern Input INPUT;

bool window_create(size_t width, size_t height);
void window_set_drag_cursor();
void window_set_default_cursor();
void window_clear_input();
void window_update();
