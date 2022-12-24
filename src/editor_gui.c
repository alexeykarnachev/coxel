#include "editor_gui.h"

#include "components/gui_rect.h"
#include "components/gui_text.h"
#include "components/transformation.h"
#include "ecs.h"
#include "la.h"
#include "window.h"

static PaneW PANES[256];
static ButtonW BUTTONS[128];
static InputW INPUTS[128];
static GUIWidget WIDGETS[256];

static size_t N_PANES = 0;
static size_t N_BUTTONS = 0;
static size_t N_INPUTS = 0;
static size_t N_WIDGETS = 0;

enum LAYER {
    LAYER_HANDLE,
    LAYER_TEXT,
    LAYER_BUTTON,
    LAYER_SELECTION,
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
GUI widgets: pane, button, text input, ...
*/
static PaneW* create_pane(
    size_t x,
    size_t y,
    float width,
    float height,
    Vec4 rect_color,
    Vec4 handle_rect_color
) {
    size_t handles_size = 10;
    size_t pane_rect = create_rect(
        -1, x, y, LAYER_PANE, width, height, rect_color, GUI_TAG_PANE
    );
    size_t resize_rect = create_rect(
        pane_rect,
        width - handles_size,
        height - handles_size,
        LAYER_HANDLE,
        handles_size,
        handles_size,
        handle_rect_color,
        GUI_TAG_RESIZE
    );
    size_t drag_rect = create_rect(
        pane_rect,
        0,
        0,
        LAYER_HANDLE,
        width,
        handles_size,
        handle_rect_color,
        GUI_TAG_DRAG
    );
    size_t scroll_rect = create_rect(
        pane_rect,
        width - handles_size,
        handles_size * 1.5,
        LAYER_HANDLE,
        handles_size,
        height - handles_size * 3,
        handle_rect_color,
        GUI_TAG_SCROLL
    );

    PaneW* pane = &PANES[N_PANES++];
    pane->width = width;
    pane->height = height;
    pane->rect = pane_rect;
    pane->resize_rect = resize_rect;
    pane->drag_rect = drag_rect;
    pane->scroll_rect = scroll_rect;

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
    int parent,
    char* label,
    size_t x,
    size_t y,
    size_t width,
    size_t border_width,
    size_t cursor_width,
    size_t font_size,
    Vec3 text_color,
    Vec4 input_rect_color,
    Vec4 selection_color,
    Vec4 cursor_color
) {
    float input_char_width = GUI_FONT_ASPECT * (float)font_size;
    size_t input_rect_hight = font_size + border_width * 2;

    size_t input_rect = create_rect(
        parent,
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
    size_t selection_rect = create_rect(
        input_text,
        0,
        0,
        LAYER_SELECTION,
        0,
        font_size,
        selection_color,
        GUI_TAG_SELECTION
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
    input->selection_rect = selection_rect;
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
        180,
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
        180,
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
        180,
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
        pane->rect,
        "Test_0",
        100,
        200,
        90,
        5,
        2,
        22,
        vec3(0.8, 0.8, 0.8),
        vec4(0.05, 0.05, 0.05, 1.0),
        vec4(0.5, 0.2, 0.0, 1.0),
        vec4(1.0, 1.0, 1.0, 1.0)
    );
    create_input(
        pane->rect,
        "Test_1",
        100,
        240,
        90,
        5,
        2,
        22,
        vec3(0.8, 0.8, 0.8),
        vec4(0.05, 0.05, 0.05, 1.0),
        vec4(0.5, 0.2, 0.0, 1.0),
        vec4(1.0, 1.0, 1.0, 1.0)
    );
    create_input(
        pane->rect,
        "Test_2",
        100,
        280,
        90,
        5,
        2,
        22,
        vec3(0.8, 0.8, 0.8),
        vec4(0.05, 0.05, 0.05, 1.0),
        vec4(0.5, 0.2, 0.0, 1.0),
        vec4(1.0, 1.0, 1.0, 1.0)
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

void input_set_hot_color(InputW* input) {}

void input_set_cold_color(InputW* input) {
    if (input == NULL)
        return;
    ecs_disable_component(input->cursor_rect, GUI_RECT_COMPONENT);
    ecs_disable_component(input->selection_rect, GUI_RECT_COMPONENT);
}

void input_set_active_color(InputW* input) {
    if (input == NULL)
        return;
    ecs_enable_component(input->cursor_rect, GUI_RECT_COMPONENT);
    ecs_enable_component(input->selection_rect, GUI_RECT_COMPONENT);
}

void editor_gui_create() {
    create_test_pane();
}

void pane_resize(PaneW* pane, float dx, float dy) {
    GUIRect* rect = ecs_get_gui_rect(pane->rect);
    rect->width += dx;
    rect->height += dy;

    GUIRect* resize_rect = ecs_get_gui_rect(pane->resize_rect);
    Transformation* resize_t = ecs_get_transformation(pane->resize_rect);
    resize_t->translation.data[0] = rect->width - resize_rect->width;
    resize_t->translation.data[1] = rect->height - resize_rect->height;

    GUIRect* drag_rect = ecs_get_gui_rect(pane->drag_rect);
    drag_rect->width += dx;

    float width_ratio = min(1.0, rect->width / pane->width);
    float height_ratio = min(1.0, rect->height / pane->height);
    GUIRect* scroll_rect = ecs_get_gui_rect(pane->scroll_rect);
    scroll_rect->height = rect->height * height_ratio;
    Transformation* scroll_t = ecs_get_transformation(pane->scroll_rect);
    scroll_t->translation.data[0] = rect->width - scroll_rect->width;
}

void pane_drag(PaneW* pane, float dx, float dy) {
    Transformation* pane_t = ecs_get_transformation(pane->rect);
    pane_t->translation.data[0] += dx;
    pane_t->translation.data[1] += dy;
}
