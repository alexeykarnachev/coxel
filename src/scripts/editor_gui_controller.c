#include "editor_gui_controller.h"

#include "../components/gui_rect.h"
#include "../components/gui_text.h"
#include "../ecs.h"
#include "../window.h"
#include <GLFW/glfw3.h>

static void heat_up_new_button(
    EditorGUIControllerArgs* ctx, ButtonW* button
) {
    if (ctx->hot_button != ctx->active_button) {
        button_set_cold_color(ctx->hot_button);
    }
    if (ctx->active_button != button) {
        button_set_hot_color(button);
    }
    ctx->hot_button = button;
}

static void activate_new_button(
    EditorGUIControllerArgs* ctx, ButtonW* button
) {
    button_set_cold_color(ctx->active_button);
    button_set_active_color(button);
    ctx->active_button = button;
}

static void toggle_current_hot_button(EditorGUIControllerArgs* ctx) {
    if (ctx->hot_button == ctx->active_button) {
        activate_new_button(ctx, NULL);
    } else {
        activate_new_button(ctx, ctx->hot_button);
    }
}

static void heat_up_new_input(
    EditorGUIControllerArgs* ctx, InputW* input
) {
    if (ctx->hot_input != ctx->active_input) {
        input_set_cold_color(ctx->hot_input);
    }
    if (ctx->active_input != input) {
        input_set_hot_color(input);
    }
    ctx->hot_input = input;
    if (ctx->hot_input != NULL) {
        window_set_text_input_cursor();
    } else {
        window_set_default_cursor();
    }
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

static int get_input_char_loc(InputW* input, int cursor_x) {
    GUIText* input_text = ecs_get_gui_text(input->input_text);
    Vec3 text_world_pos = ecs_get_world_position(input->input_text);
    int loc = round(
        (cursor_x - text_world_pos.data[0]) / input->glyph_width
    );
    loc = min(input_text->n_chars, max(0, loc));
    return loc;
}

static void activate_new_input(
    EditorGUIControllerArgs* ctx, InputW* input, int cursor_x
) {
    input_set_cold_color(ctx->active_input);
    input_set_active_color(input);
    ctx->active_input = input;

    if (ctx->active_input == NULL)
        return;

    int loc = get_input_char_loc(ctx->active_input, cursor_x);
    place_input_cursor_at(ctx->active_input, loc);
}

static void move_active_input_cursor_n_steps(
    EditorGUIControllerArgs* ctx, int n
) {
    InputW* input = ctx->active_input;
    if (input == NULL)
        return;

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

static void remove_char_in_active_input(EditorGUIControllerArgs* ctx) {
    InputW* input = ctx->active_input;
    if (input == NULL)
        return;

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
            remove_char_in_active_input(ctx);
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
    move_active_input_cursor_n_steps(ctx, -1);
}

static void insert_char_in_active_input(
    EditorGUIControllerArgs* ctx, char c
) {
    InputW* input = ctx->active_input;
    if (input == NULL)
        return;

    GUIRect* selection_rect = ecs_get_gui_rect(input->selection_rect);
    if (selection_rect->width > 0) {
        remove_char_in_active_input(ctx);
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
    move_active_input_cursor_n_steps(ctx, 1);
}

static void expand_active_input_selection_to(
    EditorGUIControllerArgs* ctx, int cursor_x
) {
    InputW* input = ctx->active_input;
    if (input == NULL)
        return;

    Transformation* cursor_transformation = ecs_get_transformation(
        input->cursor_rect
    );
    Transformation* selection_transformation = ecs_get_transformation(
        input->selection_rect
    );
    GUIRect* selection_rect = ecs_get_gui_rect(input->selection_rect);
    int cursor_loc = cursor_transformation->translation.data[0]
                     / input->glyph_width;

    int loc = get_input_char_loc(input, cursor_x);
    float x1 = loc * input->glyph_width;
    float x2 = cursor_transformation->translation.data[0];
    selection_transformation->translation.data[0] = min(x1, x2);
    selection_rect->width = max(x1, x2) - min(x1, x2);
}

static void editor_gui_controller_update(size_t _, void* args_p) {
    EditorGUIControllerArgs* ctx = (EditorGUIControllerArgs*)(args_p);
    window_set_default_cursor();

    int cursor_x = (int)(INPUT.cursor_x * ctx->overlay_buffer->width);
    int hot_entity = overlay_buffer_get_entity_id_at_cursor(
        ctx->overlay_buffer
    );
    int is_cursor_on_gui = hot_entity != -1;
    int hot_tag = ecs_get_tag(hot_entity);
    hot_entity = ecs_get_parent_with_component(
        hot_entity, GUI_WIDGET_COMPONENT, 1
    );

    GUIWidget* w = ecs_get_gui_widget(hot_entity);
    GUIWidget* hot_widget = w == NULL ? &NULL_WIDGET : w;

    heat_up_new_button(ctx, NULL);
    heat_up_new_input(ctx, NULL);

    if (window_check_if_mouse_pressed()) {
        activate_new_input(ctx, NULL, 0);
    } else if (window_check_if_lmb_keep_holding()) {
        expand_active_input_selection_to(ctx, cursor_x);
    } else if (window_check_if_backspace_should_be_printed()) {
        remove_char_in_active_input(ctx);
    } else if (window_check_if_left_should_be_printed()) {
        move_active_input_cursor_n_steps(ctx, -1);
    } else if (window_check_if_right_should_be_printed()) {
        move_active_input_cursor_n_steps(ctx, 1);
    } else if (window_check_if_key_should_be_printed()) {
        insert_char_in_active_input(ctx, INPUT.key_holding);
    }

    if (hot_widget->type == GUI_WIDGET_BUTTON) {
        ButtonW* hot_button = (ButtonW*)hot_widget->pointer;
        heat_up_new_button(ctx, hot_button);
        if (window_check_if_lmb_released()) {
            toggle_current_hot_button(ctx);
        }
    } else if (hot_widget->type == GUI_WIDGET_INPUT) {
        InputW* hot_input = (InputW*)hot_widget->pointer;
        heat_up_new_input(ctx, hot_input);
        if (window_check_if_lmb_pressed()) {
            activate_new_input(ctx, hot_input, cursor_x);
        }
    }

    if (is_cursor_on_gui) {
        window_clear_input();
    }
#if 0
    if (is_active_input_processing_key) {
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
    args.active_button = NULL;
    args.hot_input = NULL;
    args.active_input = NULL;
    return args;
}

Script* editor_gui_controller_create_script(EditorGUIControllerArgs* args
) {
    return script_create(editor_gui_controller_update, args);
};
