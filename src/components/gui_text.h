#pragma once
#include "../constants.h"
#include "../includes.h"
#include "../la.h"
#include "stddef.h"

typedef struct GUIText {
    size_t n_chars;
    size_t font_height;
    Vec3 color;
    unsigned int char_inds[GUI_TEXT_MAX_N_CHARS];
} GUIText;

GUIText* gui_text_create(
    unsigned char* text, Vec3 color, size_t font_height
);
