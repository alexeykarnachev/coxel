#include "gui_rect.h"

static GUIRect GUI_RECTS_ARENA[1 << 14];
static size_t N_GUI_RECTS = 0;

GUIRect* gui_rect_create(
    float width, float height, int layer, Vec4 color
) {
    GUIRect* gui_rect = &GUI_RECTS_ARENA[N_GUI_RECTS++];

    gui_rect->width = width;
    gui_rect->height = height;
    gui_rect->layer = layer;
    gui_rect->color = color;

    return gui_rect;
}
