#pragma once
#include "../la.h"

typedef struct GUIRect {
    float width;
    float height;
    Vec4 color;
} GUIRect;

GUIRect* gui_rect_create(float width, float height, Vec4 color);
