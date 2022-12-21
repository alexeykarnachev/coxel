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

    int mouse_pressed;
    int mouse_released;
    int mouse_holding;

    int key_pressed;
    int key_released;
    int key_holding;
    int key_repeating;
} Input;

extern Input INPUT;

int window_check_if_mouse_pressed();
int window_check_if_lmb_pressed();
int window_check_if_lmb_released();
int window_check_if_lmb_keep_holding();
int window_check_if_backspace_should_be_printed();
int window_check_if_left_should_be_printed();
int window_check_if_right_should_be_printed();
int window_check_if_key_should_be_printed();

bool window_create(size_t width, size_t height);
void window_set_drag_cursor();
void window_set_rotate_cursor();
void window_set_scale_cursor();
void window_set_text_input_cursor();
void window_set_default_cursor();
void window_clear_input();
void window_update();
