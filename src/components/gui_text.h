#define GUI_TEXT_PACK_SIZE 384

typedef struct GUIText {
    char* text;
    float x;
    float y;
    size_t n_glyphs;
} GUIText;


GUIText gui_text_create(char* text, float x, float y) {
    GUIText gui_text;

    gui_text.text = text;
    gui_text.x = x;
    gui_text.y = y;
    gui_text.n_glyphs = min(MAX_N_TEXT_GLYPHS, strlen(text));

    return gui_text;
}

void gui_text_pack(GUIText* gui_text, float dst[]) {
    for (size_t i = 0; i < gui_text->n_glyphs; ++i) {
        dst[i * 3] = gui_text->text[i];
        dst[i * 3 + 1] = gui_text->x + 19 * i;
        dst[i * 3 + 2] = gui_text->y;
    }
}

