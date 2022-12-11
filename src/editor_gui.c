#include "editor_gui.h"

#include "components/gui_rect.h"
#include "components/gui_text.h"
#include "components/transformation.h"
#include "ecs.h"
#include "la.h"
#include <string.h>

static Vec4 PANE_COLOR = {{0.1, 0.1, 0.1, 0.9}};
static Vec4 COLD_BUTTON_COLOR = {{0.2, 0.2, 0.2, 1.0}};
static size_t BUTTON_FONT_SIZE = 24;

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

static size_t
create_text(int parent, char* label, int x, int y, size_t font_size) {
    size_t text = ecs_create_entity(parent);
    ecs_add_component(text, GUI_TEXT_T, gui_text_create(label, font_size));
    ecs_add_component(
        text,
        TRANSFORMATION_T,
        transformation_create(vec3_ones, vec3_zeros, vec3(x, y, 0.0))
    );

    return text;
}

static size_t
create_pane(size_t x, size_t y, size_t width, size_t height) {
    size_t pane = create_rect(-1, x, y, width, height, PANE_COLOR);

    return pane;
}

static size_t create_button(
    int parent,
    char* label,
    size_t x,
    size_t y,
    size_t width,
    size_t height
) {
    size_t button
        = create_rect(parent, x, y, width, height, COLD_BUTTON_COLOR);

    size_t text_width
        = strlen(label) * (GUI_FONT_ASPECT * BUTTON_FONT_SIZE);
    size_t text = create_text(
        button,
        label,
        (width - text_width) / 2,
        (height - BUTTON_FONT_SIZE) / 2,
        BUTTON_FONT_SIZE
    );

    return button;
}

static size_t create_graphics_debug_pane(size_t x, size_t y) {
    size_t pane = create_pane(x, y, 200, 600);
    size_t test_button
        = create_button(pane, "test_button", 10, 10, 180, 50);

    return pane;
}

void editor_gui_create() {
    size_t graphics_debug_pane = create_graphics_debug_pane(10, 10);
}

void editor_gui_update() {}
