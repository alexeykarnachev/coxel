#pragma once
#include "../buffers/overlay_buffer.h"
#include "../components/gui_rect.h"
#include "../components/gui_text.h"
#include "../components/script.h"
#include "../window.h"
#include <GLFW/glfw3.h>

typedef struct EditorGUIControllerArgs {
    OverlayBuffer* overlay_buffer;

    ButtonW* hot_button;
    ButtonW* active_button;
    InputW* hot_input;
    InputW* active_input;
} EditorGUIControllerArgs;

static void button_cool_down(ButtonW* button) {
    if (button == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(button->rect);
    GUIText* text = ecs_get_gui_text(button->text);
    rect->color = button->rect_cold_color;
    text->color = button->text_cold_color;
}

static void button_heat_up(ButtonW* button) {
    if (button == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(button->rect);
    GUIText* text = ecs_get_gui_text(button->text);
    rect->color = button->rect_hot_color;
    text->color = button->text_hot_color;
}

static void button_activate(ButtonW* button) {
    if (button == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(button->rect);
    GUIText* text = ecs_get_gui_text(button->text);
    rect->color = button->rect_active_color;
    text->color = button->text_active_color;
}

static void input_cool_down(InputW* input) {
    if (input == NULL)
        return;
    ecs_disable_component(input->cursor_rect, GUI_RECT_COMPONENT);
    ecs_disable_component(input->selection_rect, GUI_RECT_COMPONENT);
}

static void input_heat_up(InputW* input) {
    if (input == NULL)
        return;
    window_set_text_input_cursor();
}

static void input_activate(InputW* input) {
    if (input == NULL)
        return;
    ecs_enable_component(input->cursor_rect, GUI_RECT_COMPONENT);
}

static void input_place_cursor_at(InputW* input, size_t pos) {
    Transformation* cursor_transformation = ecs_get_transformation(
        input->cursor_rect
    );
    Transformation* text_transformation = ecs_get_transformation(
        input->input_text
    );
    cursor_transformation->translation.data[0] = pos * input->glyph_width;
}

static void input_move_cursor_n_steps(InputW* input, int n) {
    Transformation* cursor_transformation = ecs_get_transformation(
        input->cursor_rect
    );
    int curr_pos = cursor_transformation->translation.data[0]
                   / input->glyph_width;
    GUIText* input_text = ecs_get_gui_text(input->input_text);
    int new_pos = curr_pos + n;
    new_pos = min(input_text->n_chars, max(0, new_pos));
    input_place_cursor_at(input, new_pos);
}

static void input_insert_char(InputW* input, char c) {
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
    int curr_pos = cursor_transformation->translation.data[0]
                   / input->glyph_width;
    GUIText* input_text = (GUIText*)
        COMPONENTS[GUI_TEXT_COMPONENT][input->input_text];
    if (input_text->n_chars == max_n_chars) {
        return;
    }

    for (size_t i = input_text->n_chars; i > curr_pos; --i) {
        input_text->char_inds[i] = input_text->char_inds[i - 1];
    }
    input_text->char_inds[curr_pos] = c;
    input_text->n_chars++;
    input_move_cursor_n_steps(input, 1);
}

static void input_remove_char(InputW* input) {
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
        input_place_cursor_at(input, x + w);
        selection_rect->width = 0;
        for (size_t i = 0; i < w - 1; ++i) {
            input_remove_char(input);
        }
    }

    Transformation* cursor_transformation = ecs_get_transformation(
        input->cursor_rect
    );
    int curr_pos = cursor_transformation->translation.data[0]
                   / input->glyph_width;
    if (curr_pos == 0) {
        return;
    }

    for (size_t i = curr_pos - 1; i < input_text->n_chars - 1; ++i) {
        input_text->char_inds[i] = input_text->char_inds[i + 1];
    }
    input_text->n_chars--;
    input_move_cursor_n_steps(input, -1);
}

static void editor_gui_controller_update(size_t _, void* args_p) {
    EditorGUIControllerArgs* args = (EditorGUIControllerArgs*)(args_p);

    unsigned char id = 0;
    int x = (int)(INPUT.cursor_x * args->overlay_buffer->width);
    int y = (int)(INPUT.cursor_y * args->overlay_buffer->height);
    glBindFramebuffer(GL_FRAMEBUFFER, args->overlay_buffer->fbo);
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &id);

    int entity = (int)id - 1;
    if (!ecs_is_component_enabled(entity, GUI_WIDGET_COMPONENT)) {
        return;
    }

    GUIWidget* widget = COMPONENTS[GUI_WIDGET_COMPONENT][entity];

    window_set_default_cursor();
    button_cool_down(args->hot_button);
    args->hot_button = NULL;
    args->hot_input = NULL;

    if (widget->type == GUI_WIDGET_BUTTON) {
        args->hot_button = (ButtonW*)widget->pointer;
        if (INPUT.is_mouse_left_released) {
            button_cool_down(args->active_button);
            if (args->active_button != args->hot_button) {
                args->active_button = args->hot_button;
            } else {
                args->active_button = 0;
            }
        }
    } else if (widget->type == GUI_WIDGET_INPUT) {
        args->hot_input = (InputW*)widget->pointer;
        if (INPUT.is_mouse_left_pressed) {
            if (args->active_input != args->hot_input) {
                input_cool_down(args->active_input);
                args->active_input = args->hot_input;
            }
            GUIText* input_text = ecs_get_gui_text(
                args->active_input->input_text
            );
            Vec3 text_world_pos = ecs_get_world_position(
                args->active_input->input_text
            );
            float cursor_local_pos = INPUT.cursor_x * INPUT.window_width
                                     - text_world_pos.data[0];
            int pos = round(
                cursor_local_pos / args->active_input->glyph_width
            );
            pos = min(input_text->n_chars, pos);
            input_place_cursor_at(args->active_input, pos);
        }
    }

    button_heat_up(args->hot_button);
    button_activate(args->active_button);
    input_heat_up(args->hot_input);
    input_activate(args->active_input);

    if (args->active_input != NULL && INPUT.key_pressed >= 0) {
        if (INPUT.key_pressed == GLFW_KEY_BACKSPACE) {
            input_remove_char(args->active_input);
        } else if (INPUT.key_pressed == GLFW_KEY_LEFT) {
            input_move_cursor_n_steps(args->active_input, -1);
        } else if (INPUT.key_pressed == GLFW_KEY_RIGHT) {
            input_move_cursor_n_steps(args->active_input, 1);
        } else {
            input_insert_char(args->active_input, INPUT.key_pressed);
        }
    }

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
