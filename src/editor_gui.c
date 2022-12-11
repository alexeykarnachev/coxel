#include "editor_gui.h"

#include "components/gui_rect.h"
#include "components/gui_text.h"
#include "components/transformation.h"
#include "ecs.h"
#include "la.h"
#include "window.h"
#include <string.h>

typedef struct Button {
    size_t rect;
    size_t text;
} Button;

static Vec4 PANE_COLOR = {{0.1, 0.1, 0.1, 0.9}};
static Vec4 BUTTON_COLD_COLOR = {{0.2, 0.2, 0.2, 1.0}};
static Vec4 BUTTON_HOT_COLOR = {{0.3, 0.3, 0.3, 1.0}};
static Vec4 BUTTON_ACTIVE_COLOR = {{0.8, 0.8, 0.8, 1.0}};
static Vec3 BUTTON_TEXT_COLD_COLOR = {{0.8, 0.8, 0.8}};
static Vec3 BUTTON_TEXT_HOT_COLOR = {{0.9, 0.9, 0.9}};
static Vec3 BUTTON_TEXT_ACTIVE_COLOR = {{0.2, 0.2, 0.2}};
static size_t BUTTON_FONT_SIZE = 24;

static Button BUTTONS[128];
static size_t N_BUTTONS = 0;

static int ENTITY_HOT = -1;
static int ENTITY_ACTIVE = -1;

static size_t create_rect(
    int parent, int x, int y, size_t width, size_t height, Vec4 color
) {
    size_t pane = ecs_create_entity(parent);
    ecs_add_component(
        pane, GUI_RECT_T, gui_rect_create(width, height, color)
    );
    ecs_add_component(
        pane,
        TRANSFORMATION_T,
        transformation_create(vec3_ones, vec3_zeros, vec3(x, y, 0))
    );

    return pane;
}

static size_t create_text(
    int parent, char* label, Vec3 color, int x, int y, size_t font_size
) {
    size_t text = ecs_create_entity(parent);
    ecs_add_component(
        text, GUI_TEXT_T, gui_text_create(label, color, font_size)
    );
    ecs_add_component(
        text,
        TRANSFORMATION_T,
        transformation_create(vec3_ones, vec3_zeros, vec3(x, y, 0.0))
    );

    return text;
}

static size_t create_pane(
    size_t x, size_t y, size_t width, size_t height
) {
    size_t pane = create_rect(-1, x, y, width, height, PANE_COLOR);

    return pane;
}

static Button create_button(
    int parent,
    char* label,
    size_t x,
    size_t y,
    size_t width,
    size_t height
) {
    size_t rect = create_rect(
        parent, x, y, width, height, BUTTON_COLD_COLOR
    );

    size_t text_width = strlen(label)
                        * (GUI_FONT_ASPECT * BUTTON_FONT_SIZE);
    size_t text = create_text(
        rect,
        label,
        BUTTON_TEXT_ACTIVE_COLOR,
        (width - text_width) / 2,
        (height - BUTTON_FONT_SIZE) / 2,
        BUTTON_FONT_SIZE
    );

    Button button = {rect, text};
    BUTTONS[N_BUTTONS++] = button;

    return button;
}

static size_t create_graphics_debug_pane(size_t x, size_t y) {
    size_t pane = create_pane(x, y, 200, 600);
    Button test_button_0 = create_button(
        pane, "test_button_0", 10, 10, 180, 50
    );
    Button test_button_1 = create_button(
        pane, "test_button_1", 10, 70, 180, 50
    );
    Button test_button_2 = create_button(
        pane, "test_button_2", 10, 130, 180, 50
    );

    return pane;
}

void editor_gui_create() {
    size_t graphics_debug_pane = create_graphics_debug_pane(10, 10);
}

void editor_gui_update() {
    float cursor_x = INPUT.cursor_x * INPUT.window_width;
    float cursor_y = (1.0 - INPUT.cursor_y) * INPUT.window_height;

    for (size_t i = 0; i < N_BUTTONS; ++i) {
        Button button = BUTTONS[i];
        GUIRect* rect = (GUIRect*)COMPONENTS[GUI_RECT_T][button.rect];
        GUIText* text = (GUIText*)COMPONENTS[GUI_TEXT_T][button.text];
        Vec3 position = ecs_get_world_position(button.rect);
        float button_x = position.data[0];
        float button_y = position.data[1];
        float button_w = rect->width;
        float button_h = rect->height;

        int is_button_hot = is_point_inside_rect(
            button_x, button_y, button_w, button_h, cursor_x, cursor_y
        );

        if (is_button_hot) {
            rect->color = BUTTON_HOT_COLOR;
            text->color = BUTTON_TEXT_HOT_COLOR;
            if (INPUT.mouse_left_released) {
                ENTITY_ACTIVE = ENTITY_ACTIVE == button.rect ? -1
                                                             : button.rect;
                INPUT.mouse_left_released = false;
            }
        } else {
            rect->color = BUTTON_COLD_COLOR;
            text->color = BUTTON_TEXT_COLD_COLOR;
        }

        if (button.rect == ENTITY_ACTIVE) {
            rect->color = BUTTON_ACTIVE_COLOR;
            text->color = BUTTON_TEXT_ACTIVE_COLOR;
        }
    }
}
