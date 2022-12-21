#pragma once
#include "la.h"
#include "stddef.h"

typedef enum GUIWidgetType {
    GUI_WIDGET_PANE,
    GUI_WIDGET_BUTTON,
    GUI_WIDGET_INPUT,
    GUI_WIDGET_NULL
} GUIWidgetType;

typedef struct GUIWidget {
    void* pointer;
    GUIWidgetType type;
} GUIWidget;

enum GUITag {
    GUI_TAG_CURSOR = 1 << 0,
    GUI_TAG_SELECTION = 1 << 1,
    GUI_TAG_RESIZE = 1 << 2
};
extern GUIWidget NULL_WIDGET;

typedef struct PaneW {
    size_t rect;
    size_t resize_rect;
} PaneW;

typedef struct ButtonW {
    size_t rect;
    size_t text;

    Vec3 text_cold_color;
    Vec3 text_hot_color;
    Vec3 text_active_color;
    Vec4 rect_cold_color;
    Vec4 rect_hot_color;
    Vec4 rect_active_color;
} ButtonW;

typedef struct InputW {
    size_t input_rect;
    size_t cursor_rect;
    size_t selection_rect;
    size_t label_text;
    size_t input_text;

    int is_selecting;
    float glyph_width;
} InputW;

void editor_gui_create();
void button_set_hot_color(ButtonW* button);
void button_set_cold_color(ButtonW* button);
void button_set_active_color(ButtonW* button);
void input_set_hot_color(InputW* input);
void input_set_cold_color(InputW* input);
void input_set_active_color(InputW* input);
void pane_resize_by_lower_right(PaneW* pane, size_t x, size_t y);
