#include "gui_text.h"

static GUIText GUI_TEXTS_ARENA[1 << 14];
static size_t N_GUI_TEXTS = 0;

GUIText* gui_text_create(unsigned char* text, size_t font_height) {
    GUIText* gui_text = &GUI_TEXTS_ARENA[N_GUI_TEXTS++];

    size_t n_chars = min(GUI_TEXT_MAX_N_CHARS, strlen(text));
    for (size_t i = 0; i < n_chars; ++i) {
        gui_text->char_inds[i] = text[i];
    }

    gui_text->n_chars = n_chars;
    gui_text->font_height = font_height;

    return gui_text;
}
