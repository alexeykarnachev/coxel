#include "editor_gui_controller.h"

#include "../components/gui_rect.h"
#include "../components/gui_text.h"
#include "../ecs.h"
#include "../window.h"
#include <GLFW/glfw3.h>

static void cool_down_button(ButtonW* button) {
    if (button == NULL || button->is_active)
        return;
    GUIRect* rect = ecs_get_gui_rect(button->rect);
    GUIText* text = ecs_get_gui_text(button->text);
    rect->color = button->rect_cold_color;
    text->color = button->text_cold_color;
}

static void heat_up_button(ButtonW* button) {
    if (button == NULL || button->is_active)
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
    button->is_active = 1;
}

static void deactivate_button(ButtonW* button) {
    if (button == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(button->rect);
    GUIText* text = ecs_get_gui_text(button->text);
    rect->color = button->rect_hot_color;
    text->color = button->text_hot_color;
    button->is_active = 0;
}

static void toggle_button(ButtonW* button) {
    if (button->is_active) {
        deactivate_button(button);
    } else {
        activate_button(button);
    }
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

    int hot_entity = overlay_buffer_get_entity_id_at_cursor(
        args->overlay_buffer
    );
    int is_cursor_on_gui = hot_entity != -1;
    int hot_tag = ecs_get_tag(hot_entity);
    hot_entity = ecs_get_parent_with_component(
        hot_entity, GUI_WIDGET_COMPONENT, 1
    );

    GUIWidget* w = ecs_get_gui_widget(hot_entity);
    GUIWidget* hot_widget = w == NULL ? &NULL_WIDGET : w;

    if (args->active_input != NULL) {
        if (window_check_if_mouse_pressed()) {
            cool_down_input(args->active_input);
            args->active_input = NULL;
        } else if (window_check_if_lmb_keep_holding()) {
        }
    }

    if (hot_widget->type == GUI_WIDGET_BUTTON) {
        ButtonW* hot_button = (ButtonW*)hot_widget->pointer;
        if (window_check_if_lmb_released()) {
            toggle_button(hot_button);
        } else {
            cool_down_button(args->hot_button);
            heat_up_button(hot_button);
            args->hot_button = hot_button;
        }
    }

    if (is_cursor_on_gui) {
        window_clear_input();
    }
#if 0
    

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
#endif
}

EditorGUIControllerArgs editor_gui_controller_create_default_args(
    OverlayBuffer* overlay_buffer
) {
    EditorGUIControllerArgs args = {0};
    args.overlay_buffer = overlay_buffer;
    args.hot_button = NULL;
    args.active_input = NULL;
    return args;
}

Script* editor_gui_controller_create_script(EditorGUIControllerArgs* args
) {
    return script_create(editor_gui_controller_update, args);
};
