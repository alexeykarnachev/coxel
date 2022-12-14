#include "editor_gui.h"

#include "components/gui_rect.h"
#include "components/gui_text.h"
#include "components/transformation.h"
#include "ecs.h"
#include "la.h"
#include "window.h"
#include <GLFW/glfw3.h>
#include <string.h>

typedef struct ButtonW {
    size_t rect;
    size_t text;
} ButtonW;

typedef struct InputW {
    size_t input_rect;
    size_t cursor_rect;
    size_t selection_rect;
    size_t label_text;
    size_t input_text;
    int is_selection;
} InputW;

static Vec4 PANE_COLOR = {{0.1, 0.1, 0.1, 0.9}};
static size_t PANES[256];
static size_t N_PANES = 0;

static Vec4 BUTTON_COLD_COLOR = {{0.2, 0.2, 0.2, 1.0}};
static Vec4 BUTTON_HOT_COLOR = {{0.3, 0.3, 0.3, 1.0}};
static Vec4 BUTTON_ACTIVE_COLOR = {{0.8, 0.8, 0.8, 1.0}};
static Vec3 BUTTON_LABEL_COLD_COLOR = {{0.8, 0.8, 0.8}};
static Vec3 BUTTON_LABEL_HOT_COLOR = {{0.9, 0.9, 0.9}};
static Vec3 BUTTON_LABEL_ACTIVE_COLOR = {{0.2, 0.2, 0.2}};
static size_t BUTTON_FONT_SIZE = 24;
static float BUTTON_GLYPH_WIDTH;
static ButtonW BUTTONS[128];
static size_t N_BUTTONS = 0;

static Vec4 INPUT_COLD_COLOR = {{0.05, 0.05, 0.05, 1.0}};
static Vec4 INPUT_HOT_COLOR = {{0.8, 0.8, 0.8, 1.0}};
static Vec3 INPUT_LABEL_COLD_COLOR = {{0.8, 0.8, 0.8}};
static Vec4 INPUT_SELECTION_COLOR = {{0.5, 0.2, 0.0, 1.0}};
static size_t INPUT_FONT_SIZE = 30;
static float INPUT_GLYPH_WIDTH;
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
    PANES[N_PANES++] = pane;

    return pane;
}

static ButtonW* create_button(
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

    size_t label_text_width = strlen(label) * BUTTON_GLYPH_WIDTH;
    size_t label_text = create_text(
        button_rect,
        label,
        BUTTON_LABEL_COLD_COLOR,
        (width - label_text_width) / 2,
        (height - BUTTON_FONT_SIZE) / 2,
        BUTTON_FONT_SIZE
    );

    ButtonW* button = &BUTTONS[N_BUTTONS++];
    button->rect = button_rect;
    button->text = label_text;

    return button;
}

static InputW* create_input(
    int parent, char* label, size_t x, size_t y, size_t width
) {
    size_t input_text_offset = 5;
    size_t input_rect_hight = INPUT_FONT_SIZE + 5;
    size_t cursor_height = INPUT_FONT_SIZE * 0.8;
    size_t input_rect = create_rect(
        parent, x, y, width, input_rect_hight, INPUT_COLD_COLOR
    );

    size_t label_text_width = strlen(label) * INPUT_GLYPH_WIDTH;
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
        "",
        INPUT_LABEL_COLD_COLOR,
        input_text_offset,
        (input_rect_hight - INPUT_FONT_SIZE) / 2,
        INPUT_FONT_SIZE
    );
    size_t selection_rect = create_rect(
        initial_text,
        0,
        (input_rect_hight - cursor_height) / 2,
        0,
        cursor_height,
        INPUT_SELECTION_COLOR
    );
    size_t cursor_rect = create_rect(
        initial_text,
        0,
        (input_rect_hight - cursor_height) / 2,
        2,
        cursor_height,
        INPUT_HOT_COLOR
    );
    ecs_disable_component(cursor_rect, GUI_RECT_T);

    InputW* input = &INPUTS[N_INPUTS++];
    input->input_rect = input_rect;
    input->cursor_rect = cursor_rect;
    input->selection_rect = selection_rect;
    input->label_text = label_text;
    input->input_text = initial_text;

    return input;
}

static void place_input_cursor_at(InputW* input, size_t pos) {
    Transformation* cursor_transformation = (Transformation*)
        COMPONENTS[TRANSFORMATION_T][input->cursor_rect];
    Transformation* text_transformation = (Transformation*)
        COMPONENTS[TRANSFORMATION_T][input->input_text];
    cursor_transformation->translation.data[0] = pos * INPUT_GLYPH_WIDTH;
}

static void expand_input_selection_to(InputW* input, size_t pos) {
    Transformation* cursor_transformation = (Transformation*)
        COMPONENTS[TRANSFORMATION_T][input->cursor_rect];
    int cursor_pos = cursor_transformation->translation.data[0]
                     / INPUT_GLYPH_WIDTH;

    Transformation* selection_transformation = (Transformation*)
        COMPONENTS[TRANSFORMATION_T][input->selection_rect];
    GUIRect* selection_rect = (GUIRect*)
        COMPONENTS[GUI_RECT_T][input->selection_rect];

    float x1 = pos * INPUT_GLYPH_WIDTH;
    float x2 = cursor_transformation->translation.data[0];
    selection_transformation->translation.data[0] = min(x1, x2);
    selection_rect->width = max(x1, x2) - min(x1, x2);
}

static void move_input_cursor_n_steps(InputW* input, int n) {
    Transformation* cursor_transformation = (Transformation*)
        COMPONENTS[TRANSFORMATION_T][input->cursor_rect];
    int curr_pos = cursor_transformation->translation.data[0]
                   / INPUT_GLYPH_WIDTH;

    GUIText* input_text = (GUIText*)
        COMPONENTS[GUI_TEXT_T][input->input_text];
    int new_pos = curr_pos + n;
    new_pos = min(input_text->n_chars, max(0, new_pos));
    place_input_cursor_at(input, new_pos);
}

static void remove_input_char(InputW* input) {
    GUIText* input_text = (GUIText*)
        COMPONENTS[GUI_TEXT_T][input->input_text];
    Transformation* text_transformation = (Transformation*)
        COMPONENTS[TRANSFORMATION_T][input->input_text];
    GUIRect* selection_rect = (GUIRect*)
        COMPONENTS[GUI_RECT_T][input->selection_rect];
    if (selection_rect->width > 0) {
        int text_offset = text_transformation->translation.data[0];
        Transformation* selection_transformation = (Transformation*)
            COMPONENTS[TRANSFORMATION_T][input->selection_rect];
        int x = selection_transformation->translation.data[0];
        int w = selection_rect->width;
        printf("%d, %d, %f\n", x, w, INPUT_GLYPH_WIDTH);
    }

    Transformation* cursor_transformation = (Transformation*)
        COMPONENTS[TRANSFORMATION_T][input->cursor_rect];
    int curr_pos = cursor_transformation->translation.data[0]
                   / INPUT_GLYPH_WIDTH;
    if (curr_pos == 0) {
        return;
    }

    for (size_t i = curr_pos - 1; i < input_text->n_chars - 1; ++i) {
        input_text->char_inds[i] = input_text->char_inds[i + 1];
    }
    input_text->n_chars--;
    move_input_cursor_n_steps(input, -1);
}

static void insert_input_char(InputW* input, char c) {
    GUIRect* input_rect = (GUIRect*)
        COMPONENTS[GUI_RECT_T][input->input_rect];
    Transformation* text_transformation = (Transformation*)
        COMPONENTS[TRANSFORMATION_T][input->input_text];
    int max_n_chars = min(
        (input_rect->width - text_transformation->translation.data[0] * 2)
            / INPUT_GLYPH_WIDTH,
        GUI_TEXT_MAX_N_CHARS
    );

    Transformation* cursor_transformation = (Transformation*)
        COMPONENTS[TRANSFORMATION_T][input->cursor_rect];
    int curr_pos = cursor_transformation->translation.data[0]
                   / INPUT_GLYPH_WIDTH;
    GUIText* input_text = (GUIText*)
        COMPONENTS[GUI_TEXT_T][input->input_text];
    if (input_text->n_chars == max_n_chars) {
        return;
    }

    for (size_t i = input_text->n_chars; i > curr_pos; --i) {
        input_text->char_inds[i] = input_text->char_inds[i - 1];
    }
    input_text->char_inds[curr_pos] = c;
    input_text->n_chars++;
    move_input_cursor_n_steps(input, 1);
}

static size_t create_test_pane(size_t x, size_t y) {
    size_t pane = create_pane(x, y, 200, 600);
    ButtonW* test_button_0 = create_button(
        pane, "test_button_0", 10, 10, 180, 50
    );
    ButtonW* test_button_1 = create_button(
        pane, "test_button_1", 10, 70, 180, 50
    );
    ButtonW* test_button_2 = create_button(
        pane, "test_button_2", 10, 130, 180, 50
    );
    InputW* test_input_0 = create_input(pane, "Test_0", 100, 190, 90);
    InputW* test_input_1 = create_input(pane, "Test_1", 100, 220, 90);
    InputW* test_input_2 = create_input(pane, "Test_2", 100, 250, 90);

    return pane;
}

void editor_gui_create() {
    INPUT_GLYPH_WIDTH = GUI_FONT_ASPECT * (float)INPUT_FONT_SIZE;
    BUTTON_GLYPH_WIDTH = GUI_FONT_ASPECT * (float)BUTTON_FONT_SIZE;
    size_t test_pane = create_test_pane(10, 10);
}

void editor_gui_update() {
    float cursor_x = INPUT.cursor_x * INPUT.window_width;
    float cursor_y = (1.0 - INPUT.cursor_y) * INPUT.window_height;
    int new_entity_hot = -1;
    int was_gui_interacted = 0;

    for (size_t i = 0; i < N_PANES; ++i) {
        size_t pane = PANES[i];
        GUIRect* rect = (GUIRect*)COMPONENTS[GUI_RECT_T][pane];
        Vec3 position = ecs_get_world_position(pane);
        int is_hot = is_point_inside_rect(
            position.data[0],
            position.data[1],
            rect->width,
            rect->height,
            cursor_x,
            cursor_y
        );
        was_gui_interacted |= is_hot;
    }

    for (size_t i = 0; i < N_BUTTONS; ++i) {
        ButtonW button = BUTTONS[i];
        GUIRect* rect = (GUIRect*)COMPONENTS[GUI_RECT_T][button.rect];
        GUIText* text = (GUIText*)COMPONENTS[GUI_TEXT_T][button.text];
        Vec3 position = ecs_get_world_position(button.rect);
        int was_active = (int)(ACTIVE_BUTTON == i);
        int is_hot = is_point_inside_rect(
            position.data[0],
            position.data[1],
            rect->width,
            rect->height,
            cursor_x,
            cursor_y
        );
        was_gui_interacted |= is_hot;

        if (is_hot) {
            new_entity_hot = button.rect;
        }

        int push_direction = 0;
        if (is_hot && INPUT.is_mouse_left_released) {
            push_direction = was_active ? -1 : 1;
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
            ACTIVE_BUTTON = i;
        } else if (push_direction == -1) {
            rect->color = BUTTON_HOT_COLOR;
            text->color = BUTTON_LABEL_HOT_COLOR;
            ACTIVE_BUTTON = -1;
        }
    }

    for (size_t i = 0; i < N_INPUTS; ++i) {
        InputW* input = &INPUTS[i];
        GUIRect* input_rect = (GUIRect*)
            COMPONENTS[GUI_RECT_T][input->input_rect];
        GUIRect* selection_rect = (GUIRect*)
            COMPONENTS[GUI_RECT_T][input->selection_rect];
        Vec3 position = ecs_get_world_position(input->input_rect);
        int was_active = (int)(ACTIVE_INPUT == i);
        int is_hot = is_point_inside_rect(
            position.data[0],
            position.data[1],
            input_rect->width,
            input_rect->height,
            cursor_x,
            cursor_y
        );
        was_gui_interacted |= is_hot;

        if (!was_active) {
            ecs_disable_component(input->cursor_rect, GUI_RECT_T);
            selection_rect->width = 0;
        }

        if (is_hot) {
            window_set_text_input_cursor();
            new_entity_hot = input->input_rect;
        }

        if (!INPUT.is_mouse_left_pressed) {
            input->is_selection = 0;
        }

        if (is_hot && INPUT.is_mouse_left_pressed) {
            ACTIVE_INPUT = i;
            GUIText* input_text = (GUIText*)
                COMPONENTS[GUI_TEXT_T][input->input_text];
            Vec3 text_world_pos = ecs_get_world_position(input->input_text
            );
            float cursor_local_pos = INPUT.cursor_x * INPUT.window_width
                                     - text_world_pos.data[0];
            int pos = round(cursor_local_pos / INPUT_GLYPH_WIDTH);
            pos = min(input_text->n_chars, pos);
            ecs_enable_component(input->cursor_rect, GUI_RECT_T);

            if (!input->is_selection) {
                place_input_cursor_at(input, pos);
                input->is_selection = 1;
            } else {
                expand_input_selection_to(input, pos);
            }
        }

        if (INPUT.is_mouse_left_pressed && !is_hot) {
            ecs_disable_component(input->cursor_rect, GUI_RECT_T);
            if (was_active) {
                ACTIVE_INPUT = -1;
            }
        }
    }

    was_gui_interacted |= ACTIVE_INPUT != -1;

    if (ACTIVE_INPUT != -1 && INPUT.key_pressed >= 0) {
        InputW input = INPUTS[ACTIVE_INPUT];

        if (INPUT.key_pressed == GLFW_KEY_BACKSPACE) {
            remove_input_char(&input);
        } else if (INPUT.key_pressed == GLFW_KEY_LEFT) {
            move_input_cursor_n_steps(&input, -1);
        } else if (INPUT.key_pressed == GLFW_KEY_RIGHT) {
            move_input_cursor_n_steps(&input, 1);
        } else {
            insert_input_char(&input, INPUT.key_pressed);
        }
    }

    if (new_entity_hot != HOT_ENTITY) {
        window_set_default_cursor();
        HOT_ENTITY = new_entity_hot;
    }

    if (was_gui_interacted) {
        window_clear_input();
    }
}
