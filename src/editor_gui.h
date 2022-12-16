#pragma once

typedef enum GUIWidgetType {
    GUI_WIDGET_PANE,
    GUI_WIDGET_BUTTON,
    GUI_WIDGET_INPUT
} GUIWidgetType;

typedef struct GUIWidget {
    void* widget_p;
    GUIWidgetType type;
} GUIWidget;

void editor_gui_create();

