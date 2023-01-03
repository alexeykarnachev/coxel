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

static void toggle_current_hot_button(EditorGUIControllerArgs* ctx) {
    button_set_cold_color(ctx->active_button);
    if (ctx->hot_button != ctx->active_button) {
        button_set_active_color(ctx->hot_button);
        ctx->active_button = ctx->hot_button;
    } else {
        ctx->active_button = NULL;
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

static void activate_current_hot_input(
    EditorGUIControllerArgs* ctx, int cursor_x
) {
    input_set_cold_color(ctx->active_input);
    input_set_active_color(ctx->hot_input);
    ctx->active_input = ctx->hot_input;

    if (ctx->active_input == NULL)
        return;

    window_set_text_input_cursor();
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
    GUIRect* text_selection_rect = ecs_get_gui_rect(
        input->text_selection_rect
    );
    Transformation* text_transformation = ecs_get_transformation(
        input->input_text
    );

    if (text_selection_rect->width > 0) {
        int text_offset = text_transformation->translation.data[0];
        Transformation* text_selection_transformation
            = ecs_get_transformation(input->text_selection_rect);
        int x = text_selection_transformation->translation.data[0]
                / input->glyph_width;
        int w = text_selection_rect->width / input->glyph_width;
        place_input_cursor_at(input, x + w);
        text_selection_rect->width = 0;
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

    GUIRect* selection_rect = ecs_get_gui_rect(input->text_selection_rect);
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

    window_set_text_input_cursor();
    Transformation* cursor_transformation = ecs_get_transformation(
        input->cursor_rect
    );
    Transformation* text_selection_transformation = ecs_get_transformation(
        input->text_selection_rect
    );
    GUIRect* text_selection_rect = ecs_get_gui_rect(
        input->text_selection_rect
    );
    int cursor_loc = cursor_transformation->translation.data[0]
                     / input->glyph_width;

    int loc = get_input_char_loc(input, cursor_x);
    float x1 = loc * input->glyph_width;
    float x2 = cursor_transformation->translation.data[0];
    text_selection_transformation->translation.data[0] = min(x1, x2);
    text_selection_rect->width = max(x1, x2) - min(x1, x2);
}

static void heat_up_new_pane(EditorGUIControllerArgs* ctx, PaneW* pane) {
    ctx->hot_pane = pane;
}

static int get_list_item_idx(ListW* list, int cursor_y) {
    if (list == NULL)
        return -1;
    GUIText* text = ecs_get_gui_text(list->item_texts[0]);
    Transformation* t = ecs_get_transformation(list->rect);
    float font_size = text->font_height;
    int item_idx = (int
    )((cursor_y - t->translation.data[1] - 0.5 * font_size) / font_size);
    return item_idx;
}

static void heat_up_new_list(EditorGUIControllerArgs* ctx, ListW* list) {
    list_set_cold_color(ctx->hot_list);
    int cursor_y = (int
    )((1.0 - INPUT.cursor_y) * ctx->overlay_buffer->height);
    int item_idx = get_list_item_idx(list, cursor_y);
    list_set_hot_color(list, item_idx);
    ctx->hot_list = list;
}

static void activate_current_hot_pane(EditorGUIControllerArgs* ctx) {
    ctx->active_pane = ctx->hot_pane;

    if (ctx->active_pane == NULL)
        return;
}

static void resize_active_pane(
    EditorGUIControllerArgs* ctx, float cursor_dx, float cursor_dy
) {
    PaneW* pane = ctx->active_pane;
    if (pane == NULL)
        return;

    pane_resize(pane, cursor_dx, cursor_dy);
}

static void drag_active_pane(
    EditorGUIControllerArgs* ctx, float cursor_dx, float cursor_dy
) {
    PaneW* pane = ctx->active_pane;
    if (pane == NULL)
        return;

    pane_drag(pane, cursor_dx, cursor_dy);
}

static void scroll_active_pane(
    EditorGUIControllerArgs* ctx, float cursor_dx, float cursor_dy
) {
    PaneW* pane = ctx->active_pane;
    if (pane == NULL)
        return;

    pane_scroll(pane, cursor_dx, cursor_dy);
}

static void editor_gui_controller_update(size_t _, void* args_p) {
    EditorGUIControllerArgs* ctx = (EditorGUIControllerArgs*)(args_p);
    window_set_default_cursor();

    int cursor_x = (int)(INPUT.cursor_x * ctx->overlay_buffer->width);
    int cursor_y = (int
    )((1.0 - INPUT.cursor_y) * ctx->overlay_buffer->height);
    float cursor_dx = INPUT.cursor_dx * ctx->overlay_buffer->width;
    float cursor_dy = INPUT.cursor_dy * ctx->overlay_buffer->height;
    int hot_entity = overlay_buffer_get_entity_id_at_cursor(
        ctx->overlay_buffer
    );
    int is_cursor_on_gui = hot_entity != -1;
    int hot_tag = ecs_get_tag(hot_entity);

    // Heat up stuff only if mouse btn is not holding
    if (!window_check_if_mouse_holding()) {
        ctx->last_hot_tag = hot_tag != 0 ? hot_tag : ctx->last_hot_tag;
        hot_entity = ecs_get_parent_with_component(
            hot_entity, GUI_WIDGET_COMPONENT, 1
        );

        // Get widget which the mouse is pointing on
        GUIWidget* w = ecs_get_gui_widget(hot_entity);
        GUIWidget* hot_widget = w == NULL ? &NULL_WIDGET : w;

        // Cool down all widgets
        heat_up_new_button(ctx, NULL);
        heat_up_new_input(ctx, NULL);
        heat_up_new_pane(ctx, NULL);
        heat_up_new_list(ctx, NULL);

        // Heat up mouse hovered widget
        if (hot_widget->type == GUI_WIDGET_BUTTON) {
            heat_up_new_button(ctx, (ButtonW*)hot_widget->pointer);
        } else if (hot_widget->type == GUI_WIDGET_INPUT) {
            heat_up_new_input(ctx, (InputW*)hot_widget->pointer);
        } else if (hot_widget->type == GUI_WIDGET_PANE) {
            heat_up_new_pane(ctx, (PaneW*)hot_widget->pointer);
        } else if (hot_widget->type == GUI_WIDGET_LIST) {
            heat_up_new_list(ctx, (ListW*)hot_widget->pointer);
        }
    }

    // Process user input from keyboard and mouse
    if (window_check_if_lmb_keep_holding()) {
        if (ctx->last_hot_tag == GUI_TAG_RESIZE) {
            resize_active_pane(ctx, cursor_dx, -cursor_dy);
        } else if (ctx->last_hot_tag == GUI_TAG_DRAG) {
            drag_active_pane(ctx, cursor_dx, -cursor_dy);
        } else if (ctx->last_hot_tag == GUI_TAG_SCROLL_V) {
            scroll_active_pane(ctx, 0, -cursor_dy);
        } else if (ctx->last_hot_tag == GUI_TAG_SCROLL_H) {
            scroll_active_pane(ctx, cursor_dx, 0);
        }
        expand_active_input_selection_to(ctx, cursor_x);
    } else if (window_check_if_lmb_released()) {
        toggle_current_hot_button(ctx);
    } else if (window_check_if_lmb_pressed()) {
        activate_current_hot_pane(ctx);
        activate_current_hot_input(ctx, cursor_x);
    } else if (window_check_if_backspace_should_be_printed()) {
        remove_char_in_active_input(ctx);
    } else if (window_check_if_left_should_be_printed()) {
        move_active_input_cursor_n_steps(ctx, -1);
    } else if (window_check_if_right_should_be_printed()) {
        move_active_input_cursor_n_steps(ctx, 1);
    } else if (window_check_if_key_should_be_printed()) {
        insert_char_in_active_input(ctx, INPUT.key_holding);
    }

    // Aufheben window inputs if gui has been interacted
    if (is_cursor_on_gui)
        window_clear_input();
}

EditorGUIControllerArgs editor_gui_controller_create_default_args(
    OverlayBuffer* overlay_buffer
) {
    EditorGUIControllerArgs args = {0};

    args.last_hot_tag = 0;

    args.overlay_buffer = overlay_buffer;
    args.hot_button = NULL;
    args.active_button = NULL;
    args.hot_input = NULL;
    args.active_input = NULL;
    args.hot_pane = NULL;
    args.active_pane = NULL;
    args.hot_list = NULL;
    return args;
}

Script* editor_gui_controller_create_script(EditorGUIControllerArgs* args
) {
    return script_create(editor_gui_controller_update, args);
};
