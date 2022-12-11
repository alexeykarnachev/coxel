#pragma once
#include "../constants.h"
#include "../includes.h"
#include "stddef.h"

typedef struct GUIText {
    unsigned int char_inds[GUI_TEXT_MAX_N_CHARS];
    size_t n_chars;
    size_t font_height;
} GUIText;

GUIText* gui_text_create(unsigned char* text, size_t font_height);
