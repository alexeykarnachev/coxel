#include "editor_gui.h"

#include "components/gui_rect.h"
#include "components/gui_text.h"
#include "components/transformation.h"
#include "ecs.h"
#include "la.h"
#include "window.h"

static PaneW PANES[16];
static ButtonW BUTTONS[64];
static InputW INPUTS[64];
static SelectorW SELECTORS[64];
static GUIWidget WIDGETS[512];
static size_t PARENT_PANES[512];

static size_t N_PANES = 0;
static size_t N_BUTTONS = 0;
static size_t N_INPUTS = 0;
static size_t N_SELECTORS = 0;
static size_t N_WIDGETS = 0;

static size_t HANDLES_SIZE = 10;

enum LAYER {
    LAYER_HANDLE,
    LAYER_TEXT,
    LAYER_SELECTOR,
    LAYER_BUTTON,
    LAYER_TEXT_SELECTION,
    LAYER_CURSOR,
    LAYER_INPUT,
    LAYER_PANE
};

GUIWidget NULL_WIDGET = {NULL, GUI_WIDGET_NULL};

/*
ECS Primitives: rect, text, ...
*/
static size_t create_rect(
    int parent,
    int x,
    int y,
    int layer,
    float width,
    float height,
    Vec4 color,
    int tag
) {
    size_t rect = ecs_create_entity(parent);
    ecs_add_component(
        rect,
        GUI_RECT_COMPONENT,
        gui_rect_create(width, height, layer, color)
    );
    ecs_add_component(
        rect,
        TRANSFORMATION_COMPONENT,
        transformation_create(vec3_ones, vec3_zeros, vec3(x, y, 0))
    );
    ecs_set_tag(rect, tag);

    return rect;
}

static size_t create_text(
    int parent,
    char* label,
    int x,
    int y,
    int layer,
    size_t font_size,
    Vec3 color,
    int tag
) {
    size_t text = ecs_create_entity(parent);
    ecs_add_component(
        text,
        GUI_TEXT_COMPONENT,
        gui_text_create(label, layer, color, font_size)
    );
    ecs_add_component(
        text,
        TRANSFORMATION_COMPONENT,
        transformation_create(vec3_ones, vec3_zeros, vec3(x, y, 0.0))
    );
    ecs_set_tag(text, tag);

    return text;
}

/*
GUI widgets: pane, button, text input, selector ...
*/
static PaneW* create_pane(
    size_t x,
    size_t y,
    float width,
    float height,
    Vec4 rect_color,
    Vec4 handle_rect_color
) {
    size_t pane_rect = create_rect(
        -1, x, y, LAYER_PANE, width, height, rect_color, GUI_TAG_PANE
    );
    size_t resize_rect = create_rect(
        pane_rect,
        width - HANDLES_SIZE,
        height - HANDLES_SIZE,
        LAYER_HANDLE,
        HANDLES_SIZE,
        HANDLES_SIZE,
        handle_rect_color,
        GUI_TAG_RESIZE
    );
    size_t drag_rect = create_rect(
        pane_rect,
        0,
        0,
        LAYER_HANDLE,
        width,
        HANDLES_SIZE,
        handle_rect_color,
        GUI_TAG_DRAG
    );
    size_t scroll_h_rect = create_rect(
        pane_rect,
        HANDLES_SIZE * 0.5,
        height - HANDLES_SIZE,
        LAYER_HANDLE,
        width - HANDLES_SIZE * 2,
        HANDLES_SIZE,
        handle_rect_color,
        GUI_TAG_SCROLL_H
    );
    size_t scroll_v_rect = create_rect(
        pane_rect,
        width - HANDLES_SIZE,
        HANDLES_SIZE * 1.5,
        LAYER_HANDLE,
        HANDLES_SIZE,
        height - HANDLES_SIZE * 3,
        handle_rect_color,
        GUI_TAG_SCROLL_V
    );

    PaneW* pane = &PANES[N_PANES];
    pane->id = N_PANES++;
    pane->width = width;
    pane->height = height;
    pane->rect = pane_rect;
    pane->resize_rect = resize_rect;
    pane->drag_rect = drag_rect;
    pane->scroll_h_rect = scroll_h_rect;
    pane->scroll_v_rect = scroll_v_rect;

    GUIWidget* widget = &WIDGETS[N_WIDGETS++];
    widget->pointer = pane;
    widget->type = GUI_WIDGET_PANE;
    ecs_add_component(pane->rect, GUI_WIDGET_COMPONENT, widget);
    return pane;
}

static ButtonW* create_button(
    PaneW* pane,
    char* text,
    size_t x,
    size_t y,
    size_t width,
    size_t height,
    size_t font_size,
    Vec4 rect_cold_color,
    Vec4 rect_hot_color,
    Vec4 rect_active_color,
    Vec3 text_cold_color,
    Vec3 text_hot_color,
    Vec3 text_active_color
) {
    float char_width = GUI_FONT_ASPECT * (float)font_size;
    size_t text_width = strlen(text) * char_width;

    ButtonW* button = &BUTTONS[N_BUTTONS++];
    button->rect = create_rect(
        pane->rect, x, y, LAYER_BUTTON, width, height, rect_cold_color, -1
    );
    button->text = create_text(
        button->rect,
        text,
        (width - text_width) / 2,
        (height - font_size) / 2,
        LAYER_TEXT,
        font_size,
        text_cold_color,
        -1
    );
    button->rect_cold_color = rect_cold_color;
    button->rect_hot_color = rect_hot_color;
    button->rect_active_color = rect_active_color;
    button->text_cold_color = text_cold_color;
    button->text_hot_color = text_hot_color;
    button->text_active_color = text_active_color;

    GUIWidget* widget = &WIDGETS[N_WIDGETS++];
    widget->pointer = button;
    widget->type = GUI_WIDGET_BUTTON;
    ecs_add_component(button->rect, GUI_WIDGET_COMPONENT, widget);
    return button;
}

static InputW* create_input(
    PaneW* pane,
    char* label,
    size_t x,
    size_t y,
    size_t width,
    size_t border_width,
    size_t cursor_width,
    size_t font_size,
    Vec3 text_color,
    Vec4 input_rect_color,
    Vec4 text_selection_color,
    Vec4 cursor_color
) {
    float input_char_width = GUI_FONT_ASPECT * (float)font_size;
    size_t input_rect_hight = font_size + border_width * 2;

    size_t input_rect = create_rect(
        pane->rect,
        x,
        y,
        LAYER_INPUT,
        width,
        input_rect_hight,
        input_rect_color,
        -1
    );
    size_t label_text = create_text(
        input_rect,
        label,
        -(strlen(label) * input_char_width) - border_width,
        (input_rect_hight - font_size) / 2,
        LAYER_TEXT,
        font_size,
        text_color,
        -1
    );
    size_t input_text = create_text(
        input_rect,
        "TEST!",
        border_width,
        (input_rect_hight - font_size) / 2,
        LAYER_TEXT,
        font_size,
        text_color,
        -1
    );
    size_t text_selection_rect = create_rect(
        input_text,
        0,
        0,
        LAYER_TEXT_SELECTION,
        0,
        font_size,
        text_selection_color,
        GUI_TAG_TEXT_SELECTION
    );
    size_t cursor_rect = create_rect(
        input_text,
        0,
        0,
        LAYER_CURSOR,
        cursor_width,
        font_size,
        cursor_color,
        GUI_TAG_CURSOR
    );

    ecs_disable_component(cursor_rect, GUI_RECT_COMPONENT);

    InputW* input = &INPUTS[N_INPUTS++];
    input->input_rect = input_rect;
    input->cursor_rect = cursor_rect;
    input->text_selection_rect = text_selection_rect;
    input->label_text = label_text;
    input->input_text = input_text;
    input->is_selecting = 0;
    input->glyph_width = GUI_FONT_ASPECT * font_size;

    GUIWidget* widget = &WIDGETS[N_WIDGETS++];
    widget->pointer = input;
    widget->type = GUI_WIDGET_INPUT;
    ecs_add_component(input->input_rect, GUI_WIDGET_COMPONENT, widget);
    return input;
}

static SelectorW* create_selector(
    PaneW* pane,
    size_t x,
    size_t y,
    size_t width,
    size_t border_width,
    size_t height,
    size_t font_size,
    char* option_texts[],
    size_t n_options,
    Vec4 rect_cold_color,
    Vec4 rect_hot_color,
    Vec4 rect_active_color,
    Vec3 text_cold_color,
    Vec3 text_hot_color,
    Vec3 text_active_color
) {
    float char_width = GUI_FONT_ASPECT * (float)font_size;
    size_t text_width = strlen(option_texts[0]) * char_width;

    SelectorW* selector = &SELECTORS[N_SELECTORS++];
    selector->rect = create_rect(
        pane->rect,
        x,
        y,
        LAYER_SELECTOR,
        width,
        height,
        rect_cold_color,
        -1
    );
    selector->text = create_text(
        selector->rect,
        option_texts[0],
        border_width,
        (height - font_size) / 2,
        LAYER_TEXT,
        font_size,
        text_cold_color,
        -1
    );

    selector->n_options = n_options;
    selector->selected_option = 0;
    selector->options_rect = create_rect(
        selector->rect,
        0,
        height,
        LAYER_SELECTOR,
        width,
        font_size * n_options,
        rect_cold_color,
        -1
    );

    for (size_t i = 0; i < n_options; ++i) {
        selector->option_rects[i] = create_rect(
            selector->options_rect,
            0,
            i * font_size,
            LAYER_SELECTOR,
            width,
            font_size,
            rect_cold_color,
            -1
        );
        selector->option_texts[i] = create_text(
            selector->option_rects[i],
            option_texts[i],
            border_width,
            0.0,
            LAYER_TEXT,
            font_size,
            text_cold_color,
            -1
        );
    }

    ecs_disable_entity(selector->options_rect);

    selector->text_cold_color = text_cold_color;
    selector->text_hot_color = text_hot_color;
    selector->text_active_color = text_active_color;
    selector->rect_cold_color = rect_cold_color;
    selector->rect_hot_color = rect_hot_color;
    selector->rect_active_color = rect_active_color;

    GUIWidget* widget = &WIDGETS[N_WIDGETS++];
    widget->pointer = selector;
    widget->type = GUI_WIDGET_SELECTOR;
    ecs_add_component(selector->rect, GUI_WIDGET_COMPONENT, widget);
    return selector;
}

/*
Pane presets
*/
static PaneW* create_test_pane() {
    PaneW* pane = create_pane(
        10,
        10,
        200.0,
        600.0,
        vec4(0.1, 0.1, 0.1, 0.9),
        vec4(0.5, 0.5, 0.5, 1.0)
    );

    create_button(
        pane,
        "test_button_0",
        10,
        10,
        170,
        50,
        24,
        vec4(0.2, 0.2, 0.2, 1.0),
        vec4(0.3, 0.3, 0.3, 1.0),
        vec4(0.8, 0.8, 0.8, 1.0),
        vec3(0.8, 0.8, 0.8),
        vec3(0.8, 0.8, 0.8),
        vec3(0.2, 0.2, 0.2)
    );
    create_button(
        pane,
        "test_button_1",
        10,
        70,
        170,
        50,
        24,
        vec4(0.2, 0.2, 0.2, 1.0),
        vec4(0.3, 0.3, 0.3, 1.0),
        vec4(0.8, 0.8, 0.8, 1.0),
        vec3(0.8, 0.8, 0.8),
        vec3(0.8, 0.8, 0.8),
        vec3(0.2, 0.2, 0.2)
    );
    create_button(
        pane,
        "test_button_2",
        10,
        130,
        170,
        50,
        24,
        vec4(0.2, 0.2, 0.2, 1.0),
        vec4(0.3, 0.3, 0.3, 1.0),
        vec4(0.8, 0.8, 0.8, 1.0),
        vec3(0.8, 0.8, 0.8),
        vec3(0.8, 0.8, 0.8),
        vec3(0.2, 0.2, 0.2)
    );
    create_input(
        pane,
        "Test_0",
        100,
        200,
        80,
        5,
        2,
        22,
        vec3(0.8, 0.8, 0.8),
        vec4(0.05, 0.05, 0.05, 1.0),
        vec4(0.5, 0.2, 0.0, 1.0),
        vec4(1.0, 1.0, 1.0, 1.0)
    );
    create_input(
        pane,
        "Test_1",
        100,
        240,
        80,
        5,
        2,
        22,
        vec3(0.8, 0.8, 0.8),
        vec4(0.05, 0.05, 0.05, 1.0),
        vec4(0.5, 0.2, 0.0, 1.0),
        vec4(1.0, 1.0, 1.0, 1.0)
    );
    create_input(
        pane,
        "Test_2",
        100,
        280,
        80,
        5,
        2,
        22,
        vec3(0.8, 0.8, 0.8),
        vec4(0.05, 0.05, 0.05, 1.0),
        vec4(0.5, 0.2, 0.0, 1.0),
        vec4(1.0, 1.0, 1.0, 1.0)
    );

    char* option_texts[] = {"option_0", "option_1", "optiooooon_2"};
    create_selector(
        pane,
        10,
        320,
        170,
        5,
        30,
        22,
        option_texts,
        3,
        vec4(0.2, 0.2, 0.2, 1.0),
        vec4(0.3, 0.3, 0.3, 1.0),
        vec4(0.8, 0.8, 0.8, 1.0),
        vec3(0.8, 0.8, 0.8),
        vec3(0.8, 0.8, 0.8),
        vec3(0.2, 0.2, 0.2)
    );

    create_button(
        pane,
        "LOWER BUTTON",
        10,
        537,
        170,
        50,
        24,
        vec4(0.2, 0.2, 0.2, 1.0),
        vec4(0.3, 0.3, 0.3, 1.0),
        vec4(0.8, 0.8, 0.8, 1.0),
        vec3(0.8, 0.8, 0.8),
        vec3(0.8, 0.8, 0.8),
        vec3(0.2, 0.2, 0.2)
    );

    return pane;
}

void button_set_hot_color(ButtonW* button) {
    if (button == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(button->rect);
    GUIText* text = ecs_get_gui_text(button->text);
    rect->color = button->rect_hot_color;
    text->color = button->text_hot_color;
}

void button_set_cold_color(ButtonW* button) {
    if (button == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(button->rect);
    GUIText* text = ecs_get_gui_text(button->text);
    rect->color = button->rect_cold_color;
    text->color = button->text_cold_color;
}

void button_set_active_color(ButtonW* button) {
    if (button == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(button->rect);
    GUIText* text = ecs_get_gui_text(button->text);
    rect->color = button->rect_active_color;
    text->color = button->text_active_color;
}

void selector_set_hot_color(SelectorW* selector) {
    if (selector == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(selector->rect);
    GUIText* text = ecs_get_gui_text(selector->text);
    rect->color = selector->rect_hot_color;
    text->color = selector->text_hot_color;
}

void selector_set_cold_color(SelectorW* selector) {
    if (selector == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(selector->rect);
    GUIText* text = ecs_get_gui_text(selector->text);
    rect->color = selector->rect_cold_color;
    text->color = selector->text_cold_color;

    ecs_disable_entity(selector->options_rect);
}

void selector_set_active_color(SelectorW* selector) {
    if (selector == NULL)
        return;
    GUIRect* rect = ecs_get_gui_rect(selector->rect);
    GUIText* text = ecs_get_gui_text(selector->text);
    rect->color = selector->rect_active_color;
    text->color = selector->text_active_color;

    ecs_enable_entity(selector->options_rect);
}

void input_set_hot_color(InputW* input) {}

void input_set_cold_color(InputW* input) {
    if (input == NULL)
        return;
    ecs_disable_component(input->cursor_rect, GUI_RECT_COMPONENT);
    ecs_disable_component(input->text_selection_rect, GUI_RECT_COMPONENT);
}

void input_set_active_color(InputW* input) {
    if (input == NULL)
        return;
    ecs_enable_component(input->cursor_rect, GUI_RECT_COMPONENT);
    ecs_enable_component(input->text_selection_rect, GUI_RECT_COMPONENT);
}

void editor_gui_create() {
    create_test_pane();
}

static void scroll_pane_view(PaneW* pane, float dx, float dy) {
    for (size_t i = 0; i < N_WIDGETS; ++i) {
        GUIWidget widget = WIDGETS[i];
        size_t parent_pane_id = PARENT_PANES[i];
        if (parent_pane_id != pane->id) {
            continue;
        }

        Transformation* t;
        if (widget.type == GUI_WIDGET_BUTTON) {
            ButtonW* button = (ButtonW*)widget.pointer;
            t = ecs_get_transformation(button->rect);
        } else if (widget.type == GUI_WIDGET_INPUT) {
            InputW* input = (InputW*)widget.pointer;
            t = ecs_get_transformation(input->input_rect);
        } else {
            continue;
        }

        t->translation.data[0] += dx;
        t->translation.data[1] += dy;
    }
}

static void resize_pane_widgets(PaneW* pane, float dx) {
    for (size_t i = 0; i < N_WIDGETS; ++i) {
        GUIWidget widget = WIDGETS[i];
        size_t parent_pane_id = PARENT_PANES[i];
        if (parent_pane_id != pane->id) {
            continue;
        }

        GUIRect* r;
        if (widget.type == GUI_WIDGET_BUTTON) {
            ButtonW* button = (ButtonW*)widget.pointer;
            r = ecs_get_gui_rect(button->rect);
        } else if (widget.type == GUI_WIDGET_INPUT) {
            InputW* input = (InputW*)widget.pointer;
            r = ecs_get_gui_rect(input->input_rect);
        } else {
            continue;
        }

        r->width += dx;
    }
}

void pane_scroll(PaneW* pane, float dx, float dy) {
    GUIRect* pane_rect = ecs_get_gui_rect(pane->rect);

    float x_shift = 0;
    if (dx != 0) {
        GUIRect* rect = ecs_get_gui_rect(pane->scroll_h_rect);
        Transformation* t = ecs_get_transformation(pane->scroll_h_rect);
        float start_pos = t->translation.data[0];
        float max_pos = pane_rect->width - rect->width
                        - 1.5 * HANDLES_SIZE;
        float min_pos = 0.5 * HANDLES_SIZE;
        float range = max_pos - min_pos;
        if (range != 0) {
            float curr_pos = max(min(start_pos + dx, max_pos), min_pos);
            t->translation.data[0] = curr_pos;
            x_shift = ((start_pos - curr_pos) / range)
                      * (pane->width - pane_rect->width);
        }
    }

    float y_shift = 0;
    if (dy != 0) {
        GUIRect* rect = ecs_get_gui_rect(pane->scroll_v_rect);
        Transformation* t = ecs_get_transformation(pane->scroll_v_rect);
        float start_pos = t->translation.data[1];
        float max_pos = pane_rect->height - rect->height
                        - 1.5 * HANDLES_SIZE;
        float min_pos = 1.5 * HANDLES_SIZE;
        float range = max_pos - min_pos;
        if (range != 0) {
            float curr_pos = max(min(start_pos + dy, max_pos), min_pos);
            t->translation.data[1] = curr_pos;
            y_shift = ((start_pos - curr_pos) / range)
                      * (pane->height - pane_rect->height);
        }
    }

    scroll_pane_view(pane, x_shift, y_shift);
}

void pane_resize(PaneW* pane, float dx, float dy) {
    GUIRect* rect = ecs_get_gui_rect(pane->rect);
    GUIRect* scroll_v_rect = ecs_get_gui_rect(pane->scroll_v_rect);
    GUIRect* scroll_h_rect = ecs_get_gui_rect(pane->scroll_h_rect);
    Transformation* scroll_v_t = ecs_get_transformation(pane->scroll_v_rect
    );
    Transformation* scroll_h_t = ecs_get_transformation(pane->scroll_h_rect
    );

    // Calculate vertical scroll movement:
    float y_scroll_top_shift_old = scroll_v_t->translation.data[1]
                                   - 1.5 * HANDLES_SIZE;
    float y_scroll_range_old = rect->height - scroll_v_rect->height
                               - 3.0 * HANDLES_SIZE;
    float y_fog_size_old = pane->height - rect->height;
    float y_fog_top_size_old = y_scroll_range_old != 0.0
                                   ? y_fog_size_old
                                         * (y_scroll_top_shift_old
                                            / y_scroll_range_old)
                                   : 0.0;
    float y_fog_bot_size_old = y_fog_size_old - y_fog_top_size_old;

    float y_view_size_new = rect->height + dy - 3.0 * HANDLES_SIZE;
    float y_fog_bot_size_new = y_fog_bot_size_old - dy;
    float y_fog_top_size_new = max(
        0.0, y_fog_top_size_old + min(0.0, y_fog_bot_size_new)
    );
    float y_fog_top_shrink = y_fog_top_size_old - y_fog_top_size_new;
    y_fog_bot_size_new += y_fog_top_shrink;
    float y_fog_size_new = y_fog_top_size_new + y_fog_bot_size_new;
    float y_handle_size_new = y_view_size_new
                              * min(
                                  1.0,
                                  y_view_size_new
                                      / (pane->height - 3.0 * HANDLES_SIZE)
                              );
    float y_scroll_range_new = y_view_size_new - y_handle_size_new;
    float y_scroll_top_shift_new = y_fog_size_new != 0.0
                                       ? y_scroll_range_new
                                             * (y_fog_top_size_new
                                                / y_fog_size_new)
                                       : 0.0;

    // Calculate horizontal scroll movement:
    float x_scroll_top_shift_old = scroll_h_t->translation.data[0]
                                   - 0.5 * HANDLES_SIZE;
    float x_scroll_range_old = rect->width - scroll_h_rect->width
                               - 2.0 * HANDLES_SIZE;
    float x_fog_size_old = pane->width - rect->width;
    float x_fog_top_size_old = x_scroll_range_old != 0.0
                                   ? x_fog_size_old
                                         * (x_scroll_top_shift_old
                                            / x_scroll_range_old)
                                   : 0.0;
    float x_fog_bot_size_old = x_fog_size_old - x_fog_top_size_old;

    float x_view_size_new = rect->width + dx - 2.0 * HANDLES_SIZE;
    float x_fog_bot_size_new = x_fog_bot_size_old - dx;
    float x_fog_top_size_new = max(
        0.0, x_fog_top_size_old + min(0.0, x_fog_bot_size_new)
    );
    float x_fog_top_shrink = x_fog_top_size_old - x_fog_top_size_new;
    x_fog_bot_size_new += x_fog_top_shrink;
    float x_fog_size_new = x_fog_top_size_new + x_fog_bot_size_new;
    float x_handle_size_new
        = x_view_size_new
          * min(1.0, x_view_size_new / (pane->width - 2.0 * HANDLES_SIZE));
    float x_scroll_range_new = x_view_size_new - x_handle_size_new;
    float x_scroll_top_shift_new = x_fog_size_new != 0.0
                                       ? x_scroll_range_new
                                             * (x_fog_top_size_new
                                                / x_fog_size_new)
                                       : 0.0;

    // Move drag rectangle
    rect->width += dx;
    rect->height += dy;
    GUIRect* resize_rect = ecs_get_gui_rect(pane->resize_rect);
    Transformation* resize_t = ecs_get_transformation(pane->resize_rect);
    resize_t->translation.data[0] = rect->width - resize_rect->width;
    resize_t->translation.data[1] = rect->height - resize_rect->height;
    GUIRect* drag_rect = ecs_get_gui_rect(pane->drag_rect);
    drag_rect->width += dx;

    // Move vertical scroll
    scroll_v_t->translation.data[1] = y_scroll_top_shift_new
                                      + 1.5 * HANDLES_SIZE;
    scroll_v_t->translation.data[0] += dx;
    scroll_v_rect->height = y_handle_size_new;

    // Move horizontal scroll
    scroll_h_t->translation.data[0] = x_scroll_top_shift_new
                                      + 0.5 * HANDLES_SIZE;
    scroll_h_t->translation.data[1] += dy;
    scroll_h_rect->width = x_handle_size_new;

    // Move pane content view
    scroll_pane_view(pane, x_fog_top_shrink, y_fog_top_shrink);
    // Adjust resizable widgets size
    resize_pane_widgets(pane, dx);
}

void pane_drag(PaneW* pane, float dx, float dy) {
    Transformation* pane_t = ecs_get_transformation(pane->rect);
    pane_t->translation.data[0] += dx;
    pane_t->translation.data[1] += dy;
}
