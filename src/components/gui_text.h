#pragma once
#include "stddef.h"
#include "../constants.h"
#include "../includes.h"


typedef struct GUIText {
    unsigned int char_inds[GUI_TEXT_MAX_N_CHARS];
    size_t n_chars;
    size_t font_height;
} GUIText;

GUIText* gui_text_create(unsigned char* text, size_t font_height);
