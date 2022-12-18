#include "editor_gui_controller.h"

#include "../components/gui_rect.h"
#include "../components/gui_text.h"
#include "../ecs.h"
#include "../window.h"
#include <GLFW/glfw3.h>

static void cool_down_button(ButtonW* button) {
    if (button == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(button->rect);
    GUIText* text = ecs_get_gui_text(button->text);
    rect->color = button->rect_cold_color;
    text->color = button->text_cold_color;
}

static void heat_up_button(ButtonW* button) {
    if (button == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(button->rect);
    GUIText* text = ecs_get_gui_text(button->text);
    rect->color = button->rect_hot_color;
    text->color = button->text_hot_color;
}

static void activate_button(ButtonW* button) {
    if (button == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(button->rect);
    GUIText* text = ecs_get_gui_text(button->text);
    rect->color = button->rect_active_color;
    text->color = button->text_active_color;
}

static void cool_down_input(InputW* input) {
    if (input == NULL)
        return;
    ecs_disable_component(input->cursor_rect, GUI_RECT_COMPONENT);
    ecs_disable_component(input->selection_rect, GUI_RECT_COMPONENT);
}

static void heat_up_input(InputW* input) {
    if (input == NULL)
        return;
    window_set_text_input_cursor();
}

static void activate_input(InputW* input) {
    if (input == NULL)
        return;
    ecs_enable_component(input->cursor_rect, GUI_RECT_COMPONENT);
    ecs_enable_component(input->selection_rect, GUI_RECT_COMPONENT);
}

static void place_input_cursor_at(InputW* input, size_t char_loc) {
    Transformation* cursor_transformation = ecs_get_transformation(
        input->cursor_rect
    );
    Transformation* text_transformation = ecs_get_transformation(
        input->input_text
    );
    cursor_transformation->translation.data[0] = char_loc
                                                 * input->glyph_width;
}

static void move_input_cursor_n_steps(InputW* input, int n) {
    Transformation* cursor_transformation = ecs_get_transformation(
        input->cursor_rect
    );
    int curr_loc = cursor_transformation->translation.data[0]
                   / input->glyph_width;
    GUIText* input_text = ecs_get_gui_text(input->input_text);
    int new_loc = curr_loc + n;
    new_loc = min(input_text->n_chars, max(0, new_loc));
    place_input_cursor_at(input, new_loc);
}

static void remove_input_char(InputW* input) {
    GUIText* input_text = ecs_get_gui_text(input->input_text);
    GUIRect* selection_rect = ecs_get_gui_rect(input->selection_rect);
    Transformation* text_transformation = ecs_get_transformation(
        input->input_text
    );

    if (selection_rect->width > 0) {
        int text_offset = text_transformation->translation.data[0];
        Transformation* selection_transformation = ecs_get_transformation(
            input->selection_rect
        );
        int x = selection_transformation->translation.data[0]
                / input->glyph_width;
        int w = selection_rect->width / input->glyph_width;
        place_input_cursor_at(input, x + w);
        selection_rect->width = 0;
        for (size_t i = 0; i < w - 1; ++i) {
            remove_input_char(input);
        }
    }

    Transformation* cursor_transformation = ecs_get_transformation(
        input->cursor_rect
    );
    int curr_loc = cursor_transformation->translation.data[0]
                   / input->glyph_width;
    if (curr_loc == 0) {
        return;
    }

    for (size_t i = curr_loc - 1; i < input_text->n_chars - 1; ++i) {
        input_text->char_inds[i] = input_text->char_inds[i + 1];
    }
    input_text->n_chars--;
    move_input_cursor_n_steps(input, -1);
}

static void insert_input_char(InputW* input, char c) {
    GUIRect* selection_rect = ecs_get_gui_rect(input->selection_rect);
    if (selection_rect->width > 0) {
        remove_input_char(input);
    }

    GUIRect* input_rect = ecs_get_gui_rect(input->input_rect);
    Transformation* text_transformation = ecs_get_transformation(
        input->input_text
    );
    int max_n_chars = min(
        (input_rect->width - text_transformation->translation.data[0] * 2)
            / input->glyph_width,
        GUI_TEXT_MAX_N_CHARS
    );

    Transformation* cursor_transformation = ecs_get_transformation(
        input->cursor_rect
    );
    int curr_loc = cursor_transformation->translation.data[0]
                   / input->glyph_width;
    GUIText* input_text = (GUIText*)
        COMPONENTS[GUI_TEXT_COMPONENT][input->input_text];
    if (input_text->n_chars == max_n_chars) {
        return;
    }

    for (size_t i = input_text->n_chars; i > curr_loc; --i) {
        input_text->char_inds[i] = input_text->char_inds[i - 1];
    }
    input_text->char_inds[curr_loc] = c;
    input_text->n_chars++;
    move_input_cursor_n_steps(input, 1);
}

static void expand_input_selection_to(InputW* input, size_t char_loc) {
    Transformation* cursor_transformation = ecs_get_transformation(
        input->cursor_rect
    );
    Transformation* selection_transformation = ecs_get_transformation(
        input->selection_rect
    );
    GUIRect* selection_rect = ecs_get_gui_rect(input->selection_rect);
    int cursor_loc = cursor_transformation->translation.data[0]
                     / input->glyph_width;
    float x1 = char_loc * input->glyph_width;
    float x2 = cursor_transformation->translation.data[0];
    selection_transformation->translation.data[0] = min(x1, x2);
    selection_rect->width = max(x1, x2) - min(x1, x2);
}

int get_input_char_loc(InputW* input, float x) {
    GUIText* input_text = ecs_get_gui_text(input->input_text);
    Vec3 text_world_pos = ecs_get_world_position(input->input_text);
    int loc = round((x - text_world_pos.data[0]) / input->glyph_width);
    return min(input_text->n_chars, max(0, loc));
}

static void editor_gui_controller_update(size_t _, void* args_p) {
    EditorGUIControllerArgs* args = (EditorGUIControllerArgs*)(args_p);
    window_set_default_cursor();

    unsigned char id = 0;
    int x = (int)(INPUT.cursor_x * args->overlay_buffer->width);
    int y = (int)(INPUT.cursor_y * args->overlay_buffer->height);
    glBindFramebuffer(GL_FRAMEBUFFER, args->overlay_buffer->fbo);
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &id);

    int entity = (int)id - 1;
    int tag = ecs_get_tag(entity);
    int is_interacting_with_active_input
        = (INPUT.mouse_pressed != GLFW_MOUSE_BUTTON_LEFT
           && INPUT.mouse_holding == GLFW_MOUSE_BUTTON_LEFT
           && args->active_input != NULL)
          || (INPUT.mouse_pressed == -1 && args->active_input != NULL)
          || (tag == GUI_TAG_CURSOR || tag == GUI_TAG_SELECTION);
    if (is_interacting_with_active_input) {
        entity = args->active_input->input_rect;
    } else {
        cool_down_input(args->active_input);
        args->active_input = NULL;
    }
    int is_widget = ecs_is_component_enabled(entity, GUI_WIDGET_COMPONENT);
    int is_cursor_on_gui = entity != -1;

    cool_down_button(args->hot_button);
    cool_down_input(args->hot_input);
    args->hot_button = NULL;
    args->hot_input = NULL;

    GUIWidget* widget = COMPONENTS[GUI_WIDGET_COMPONENT][entity];
    if (widget == NULL) {
        if (is_cursor_on_gui)
            window_clear_input();
        return;
    }

    if (widget->type == GUI_WIDGET_BUTTON) {
        args->hot_button = (ButtonW*)widget->pointer;
        if (INPUT.mouse_released == GLFW_MOUSE_BUTTON_LEFT) {
            cool_down_button(args->active_button);
            if (args->active_button != args->hot_button) {
                args->active_button = args->hot_button;
            } else {
                args->active_button = 0;
            }
        }
    } else if (widget->type == GUI_WIDGET_INPUT) {
        args->hot_input = (InputW*)widget->pointer;
        int is_active_input_selection_expanding
            = args->active_input != NULL
              && INPUT.mouse_holding == GLFW_MOUSE_BUTTON_LEFT;

        if (INPUT.mouse_pressed == GLFW_MOUSE_BUTTON_LEFT) {
            if (args->active_input != args->hot_input) {
                cool_down_input(args->active_input);
                args->active_input = args->hot_input;
            }
            int loc = get_input_char_loc(args->active_input, x);
            place_input_cursor_at(args->active_input, loc);
        } else if (is_active_input_selection_expanding) {
            int loc = get_input_char_loc(args->active_input, x);
            expand_input_selection_to(args->active_input, loc);
        }
    }

    int is_active_input_cooling_down = args->active_input != NULL
                                       && args->active_input
                                              != args->hot_input
                                       && INPUT.mouse_pressed != -1;
    int is_active_input_processing_key = args->active_input != NULL
                                         && (INPUT.key_pressed >= 0
                                             || INPUT.key_repeating >= 0);

    if (is_active_input_processing_key) {
        if (INPUT.key_holding == GLFW_KEY_BACKSPACE) {
            remove_input_char(args->active_input);
        } else if (INPUT.key_holding == GLFW_KEY_LEFT) {
            move_input_cursor_n_steps(args->active_input, -1);
        } else if (INPUT.key_holding == GLFW_KEY_RIGHT) {
            move_input_cursor_n_steps(args->active_input, 1);
        } else {
            insert_input_char(args->active_input, INPUT.key_holding);
        }
    } else if (is_active_input_cooling_down) {
        cool_down_input(args->active_input);
        args->hot_input = NULL;
        args->active_input = NULL;
    }

    heat_up_button(args->hot_button);
    activate_button(args->active_button);
    heat_up_input(args->hot_input);
    activate_input(args->active_input);
    window_clear_input();
}

EditorGUIControllerArgs editor_gui_controller_create_default_args(
    OverlayBuffer* overlay_buffer
) {
    EditorGUIControllerArgs args = {0};
    args.overlay_buffer = overlay_buffer;
    return args;
}

Script* editor_gui_controller_create_script(EditorGUIControllerArgs* args
) {
    return script_create(editor_gui_controller_update, args);
};
