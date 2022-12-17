#pragma once
#include "la.h"
#include "stddef.h"

typedef enum GUIWidgetType {
    GUI_WIDGET_PANE,
    GUI_WIDGET_BUTTON,
    GUI_WIDGET_INPUT
} GUIWidgetType;

typedef struct GUIWidget {
    void* pointer;
    GUIWidgetType type;
} GUIWidget;

typedef struct PaneW {
    size_t rect;
} PaneW;

typedef struct ButtonW {
    size_t rect;
    size_t text;

    int is_active;

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
