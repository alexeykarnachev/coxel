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

    int mouse_middle_pressed;
    int mouse_left_pressed;

    int key_pressed;
    int key_released;
} Input;

extern Input INPUT;

bool window_create(size_t width, size_t height);
void window_set_drag_cursor();
void window_set_rotate_cursor();
void window_set_scale_cursor();
void window_set_text_input_cursor();
void window_set_default_cursor();
void window_clear_input();
void window_update();
