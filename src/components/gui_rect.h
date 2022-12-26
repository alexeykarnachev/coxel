#pragma once
#include "../la.h"

typedef struct GUIRect {
    float width;
    float height;
    float view_x_offset;
    float view_y_offset;
    int layer;
    Vec4 color;
} GUIRect;

GUIRect* gui_rect_create(float width, float height, int layer, Vec4 color);
