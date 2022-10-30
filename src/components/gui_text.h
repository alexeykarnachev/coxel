#define GUI_TEXT_PACK_SIZE 16 * GUI_TEXT_MAX_N_CHARS + 16

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

void gui_text_pack(GUIText* gui_text, float dst[]) {
    for (size_t i = 0; i < gui_text->n_chars; ++i) {
        memcpy(&dst[i * 4], &gui_text->char_inds[i], 1);
    }
    size_t offset = 4 * GUI_TEXT_MAX_N_CHARS;
    memcpy(&dst[offset], &gui_text->n_chars, 4);
    memcpy(&dst[offset + 1], &gui_text->height, 4);
    memcpy(&dst[offset + 2], &gui_text->x, 4);
    memcpy(&dst[offset + 3], &gui_text->y, 4);
}
