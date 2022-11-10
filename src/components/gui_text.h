typedef struct GUIText {
    unsigned int char_inds[GUI_TEXT_MAX_N_CHARS];
    int32_t n_chars;
} GUIText;

GUIText _GUI_TEXTS_ARENA[1 << 14];
size_t _N_GUI_TEXTS = 0;


GUIText* gui_text_create(unsigned char* text) {
    GUIText* gui_text = &_GUI_TEXTS_ARENA[_N_GUI_TEXTS++];

    int32_t n_chars = min(GUI_TEXT_MAX_N_CHARS, strlen(text));
    for (size_t i = 0; i < n_chars; ++i) {
        gui_text->char_inds[i] = text[i];
    }

    gui_text->n_chars = n_chars;

    return gui_text;
}

