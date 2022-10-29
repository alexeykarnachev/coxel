#define GUI_TEXT_PACK_SIZE 64

typedef struct GUIText {
    char* text;
    float x;
    float y;
} GUIText;


GUIText gui_text_create(char* text, float x, float y) {
    GUIText gui_text;

    gui_text.text = text;
    gui_text.x = x;
    gui_text.y = y;

    return gui_text;
}

void gui_text_pack(GUIText* gui_text, float dst[]) {
    // size_t size = sizeof(float);

    // memcpy(&dst[0], material->diffuse_color.data, size * 3);
    // memcpy(&dst[4], material->ambient_color.data, size * 3);
    // memcpy(&dst[8], material->specular_color.data, size * 3);
    // dst[12] = material->shininess;
}

