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
