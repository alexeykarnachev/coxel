#include "editor_gui.h"

#include "components/gui_rect.h"
#include "components/gui_text.h"
#include "components/transformation.h"
#include "ecs.h"
#include "la.h"
#include "window.h"
#include <string.h>

typedef struct ButtonW {
    size_t rect;
    size_t text;
} ButtonW;

typedef struct InputW {
    size_t rect;
    size_t label_text;
    size_t input_text;
} InputW;

static Vec4 PANE_COLOR = {{0.1, 0.1, 0.1, 0.9}};

static Vec4 BUTTON_COLD_COLOR = {{0.2, 0.2, 0.2, 1.0}};
static Vec4 BUTTON_HOT_COLOR = {{0.3, 0.3, 0.3, 1.0}};
static Vec4 BUTTON_ACTIVE_COLOR = {{0.8, 0.8, 0.8, 1.0}};
static Vec3 BUTTON_LABEL_COLD_COLOR = {{0.8, 0.8, 0.8}};
static Vec3 BUTTON_LABEL_HOT_COLOR = {{0.9, 0.9, 0.9}};
static Vec3 BUTTON_LABEL_ACTIVE_COLOR = {{0.2, 0.2, 0.2}};
static size_t BUTTON_FONT_SIZE = 24;
static ButtonW BUTTONS[128];
static size_t N_BUTTONS = 0;

static Vec4 INPUT_COLD_COLOR = {{0.05, 0.05, 0.05, 1.0}};
static Vec3 INPUT_LABEL_COLD_COLOR = {{0.8, 0.8, 0.8}};
static size_t INPUT_FONT_SIZE = 20;
static InputW INPUTS[128];
static size_t N_INPUTS = 0;

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

static ButtonW create_button(
    int parent,
    char* label,
    size_t x,
    size_t y,
    size_t width,
    size_t height
) {
    size_t button_rect = create_rect(
        parent, x, y, width, height, BUTTON_COLD_COLOR
    );

    size_t label_text_width = strlen(label)
                              * (GUI_FONT_ASPECT * BUTTON_FONT_SIZE);
    size_t label_text = create_text(
        button_rect,
        label,
        BUTTON_LABEL_COLD_COLOR,
        (width - label_text_width) / 2,
        (height - BUTTON_FONT_SIZE) / 2,
        BUTTON_FONT_SIZE
    );

    ButtonW button = {button_rect, label_text};
    BUTTONS[N_BUTTONS++] = button;

    return button;
}

static InputW create_input(
    int parent, char* label, size_t x, size_t y, size_t width
) {
    size_t input_rect_hight = INPUT_FONT_SIZE + 5;
    size_t input_rect = create_rect(
        parent, x, y, width, input_rect_hight, INPUT_COLD_COLOR
    );

    size_t label_text_width = strlen(label)
                              * (GUI_FONT_ASPECT * INPUT_FONT_SIZE);
    size_t label_text = create_text(
        input_rect,
        label,
        INPUT_LABEL_COLD_COLOR,
        -label_text_width - 5,
        (input_rect_hight - INPUT_FONT_SIZE) / 2,
        INPUT_FONT_SIZE
    );
    size_t initial_text = create_text(
        input_rect,
        "0.228",
        INPUT_LABEL_COLD_COLOR,
        5,
        (input_rect_hight - INPUT_FONT_SIZE) / 2,
        INPUT_FONT_SIZE
    );

    InputW input = {input_rect, label_text, initial_text};
    INPUTS[N_INPUTS++] = input;

    return input;
}

static size_t create_test_pane(size_t x, size_t y) {
    size_t pane = create_pane(x, y, 200, 600);
    ButtonW test_button_0 = create_button(
        pane, "test_button_0", 10, 10, 180, 50
    );
    ButtonW test_button_1 = create_button(
        pane, "test_button_1", 10, 70, 180, 50
    );
    ButtonW test_button_2 = create_button(
        pane, "test_button_2", 10, 130, 180, 50
    );
    InputW test_input_0 = create_input(pane, "Test_0", 100, 190, 90);
    InputW test_input_1 = create_input(pane, "Test_1", 100, 220, 90);
    InputW test_input_2 = create_input(pane, "Test_2", 100, 250, 90);

    return pane;
}

void editor_gui_create() {
    size_t test_pane = create_test_pane(10, 10);
}

void editor_gui_update() {
    float cursor_x = INPUT.cursor_x * INPUT.window_width;
    float cursor_y = (1.0 - INPUT.cursor_y) * INPUT.window_height;

    for (size_t i = 0; i < N_BUTTONS; ++i) {
        ButtonW button = BUTTONS[i];
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
        int was_button_active = (int)(ENTITY_ACTIVE == button.rect);
        int button_push_direction = 0;
        if (is_button_hot && INPUT.mouse_left_released) {
            button_push_direction = was_button_active ? -1 : 1;
            INPUT.mouse_left_released = 0;
        }

        if (is_button_hot && !was_button_active) {
            rect->color = BUTTON_HOT_COLOR;
            text->color = BUTTON_LABEL_HOT_COLOR;
        } else if (!was_button_active) {
            rect->color = BUTTON_COLD_COLOR;
            text->color = BUTTON_LABEL_COLD_COLOR;
        }

        if (button_push_direction == 1) {
            rect->color = BUTTON_ACTIVE_COLOR;
            text->color = BUTTON_LABEL_ACTIVE_COLOR;
            ENTITY_ACTIVE = button.rect;
        } else if (button_push_direction == -1) {
            rect->color = BUTTON_HOT_COLOR;
            text->color = BUTTON_LABEL_HOT_COLOR;
            ENTITY_ACTIVE = -1;
        }
    }
}
