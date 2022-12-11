#pragma once
#include "../la.h"

typedef struct GUIRect {
    size_t width;
    size_t height;
    Vec4 color;
} GUIRect;

GUIRect* gui_rect_create(size_t width, size_t height, Vec4 color);
