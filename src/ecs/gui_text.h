typedef struct GUIText {
    unsigned char char_inds[GUI_TEXT_MAX_N_CHARS];
    int32_t n_chars;
    int32_t height;
    float x;
    float y;
} GUIText;


GUIText gui_text_create(unsigned char* text, int32_t height, float x, float y) {
    GUIText gui_text;

    int32_t n_chars = min(GUI_TEXT_MAX_N_CHARS, strlen(text));
    for (size_t i = 0; i < n_chars; ++i) {
        gui_text.char_inds[i] = text[i];
    }

    gui_text.n_chars = n_chars;
    gui_text.height = height;
    gui_text.x = x;
    gui_text.y = y;

    return gui_text;
}

