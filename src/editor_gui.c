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
    size_t input_rect;
    size_t cursor_rect;
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
static Vec4 INPUT_HOT_COLOR = {{0.8, 0.8, 0.8, 1.0}};
static Vec3 INPUT_LABEL_COLD_COLOR = {{0.8, 0.8, 0.8}};
static size_t INPUT_FONT_SIZE = 20;
static InputW INPUTS[128];
static size_t N_INPUTS = 0;

static int HOT_ENTITY = -1;
static int ACTIVE_BUTTON = -1;
static int ACTIVE_INPUT = -1;

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
    size_t cursor_height = INPUT_FONT_SIZE * 0.8;
    size_t input_rect = create_rect(
        parent, x, y, width, input_rect_hight, INPUT_COLD_COLOR
    );
    size_t cursor_rect = create_rect(
        input_rect,
        0,
        (input_rect_hight - cursor_height) / 2,
        2,
        cursor_height,
        INPUT_HOT_COLOR
    );
    ecs_disable_component(cursor_rect, GUI_RECT_T);

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
        "A",
        INPUT_LABEL_COLD_COLOR,
        5,
        (input_rect_hight - INPUT_FONT_SIZE) / 2,
        INPUT_FONT_SIZE
    );

    InputW input = {input_rect, cursor_rect, label_text, initial_text};
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
    int new_entity_hot = -1;
    int mouse_left_released = 0;

    for (size_t i = 0; i < N_BUTTONS; ++i) {
        ButtonW button = BUTTONS[i];
        GUIRect* rect = (GUIRect*)COMPONENTS[GUI_RECT_T][button.rect];
        GUIText* text = (GUIText*)COMPONENTS[GUI_TEXT_T][button.text];
        Vec3 position = ecs_get_world_position(button.rect);
        int was_active = (int)(ACTIVE_BUTTON == button.rect);
        int is_hot = is_point_inside_rect(
            position.data[0],
            position.data[1],
            rect->width,
            rect->height,
            cursor_x,
            cursor_y
        );
        if (is_hot) {
            new_entity_hot = button.rect;
        }

        int push_direction = 0;
        if (is_hot && INPUT.mouse_left_released) {
            push_direction = was_active ? -1 : 1;
            mouse_left_released = 1;
        }

        if (is_hot && !was_active) {
            rect->color = BUTTON_HOT_COLOR;
            text->color = BUTTON_LABEL_HOT_COLOR;
        } else if (!was_active) {
            rect->color = BUTTON_COLD_COLOR;
            text->color = BUTTON_LABEL_COLD_COLOR;
        }

        if (push_direction == 1) {
            rect->color = BUTTON_ACTIVE_COLOR;
            text->color = BUTTON_LABEL_ACTIVE_COLOR;
            ACTIVE_BUTTON = button.rect;
        } else if (push_direction == -1) {
            rect->color = BUTTON_HOT_COLOR;
            text->color = BUTTON_LABEL_HOT_COLOR;
            ACTIVE_BUTTON = -1;
        }
    }

    for (size_t i = 0; i < N_INPUTS; ++i) {
        InputW input = INPUTS[i];
        GUIRect* input_rect = (GUIRect*)
            COMPONENTS[GUI_RECT_T][input.input_rect];
        GUIText* input_text = (GUIText*)
            COMPONENTS[GUI_TEXT_T][input.input_text];
        Vec3 position = ecs_get_world_position(input.input_rect);
        int was_active = (int)(ACTIVE_INPUT == input.input_rect);
        int is_hot = is_point_inside_rect(
            position.data[0],
            position.data[1],
            input_rect->width,
            input_rect->height,
            cursor_x,
            cursor_y
        );
        if (is_hot) {
            window_set_text_input_cursor();
            new_entity_hot = input.input_rect;
        }

        if (is_hot && INPUT.mouse_left_released) {
            ACTIVE_INPUT = input.input_rect;
            mouse_left_released = 1;
            Transformation* cursor_transformation = (Transformation*)
                COMPONENTS[TRANSFORMATION_T][input.cursor_rect];
            Transformation* text_transformation = (Transformation*)
                COMPONENTS[TRANSFORMATION_T][input.input_text];
            cursor_transformation->translation.data[0]
                = input_text->n_chars * (GUI_FONT_ASPECT * INPUT_FONT_SIZE)
                  + text_transformation->translation.data[0];
            ecs_enable_component(input.cursor_rect, GUI_RECT_T);
        }

        if (INPUT.mouse_left_released && !is_hot) {
            ecs_disable_component(input.cursor_rect, GUI_RECT_T);
        }
    }

    if (new_entity_hot != HOT_ENTITY) {
        window_set_default_cursor();
        HOT_ENTITY = new_entity_hot;
    }

    if (mouse_left_released) {
        INPUT.mouse_left_released = 0;
    }
}
