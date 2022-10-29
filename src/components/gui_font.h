#define GUI_FONT_PACK_SIZE 16 * MAX_N_FONT_GLYPHS

typedef struct GUIGlyph {
    size_t id;
    size_t x;
    size_t y;
    size_t width;
    size_t height;
    int32_t xoffset;
    int32_t yoffset;
} GUIGlyph;

typedef struct GUIFont {
    GUIGlyph glyphs[MAX_N_FONT_GLYPHS]; 
    Sprite sprite;
    size_t font_size;
    size_t n_glyphs;
} GUIFont;


bool gui_font_create_from_file(GUIFont* font, char* fnt_file_path, char* sprite_file_path) {
    clear_struct(font);

    size_t n_bytes;
    char* bytes = read_cstr_file(fnt_file_path, "rb", &n_bytes);
    
    char* magic[3];
    memcpy(magic, bytes, 3);

    if (memcmp(magic, "BMF", 3) != 0) goto fail;
    if (bytes[3] != 3) goto fail;

    char* blocks[4];
    int block_sizes[4];
    size_t offset = 4;

    for (size_t i = 0; i < 4; ++i) {
        if (bytes[offset] != i + 1) goto fail;
        memcpy(&block_sizes[i], &bytes[offset + 1], 4);
        offset += 5;
        blocks[i] = malloc(block_sizes[i]);
        memcpy(blocks[i], &bytes[offset], block_sizes[i]);
        offset += block_sizes[i];
    }

    size_t n_glyphs;

    char* block;
    block = blocks[0];
    font->font_size = block[0];

    block = blocks[3];
    n_glyphs = block_sizes[3] / 20;
    font->n_glyphs = n_glyphs;
    if (n_glyphs > MAX_N_FONT_GLYPHS) {
        fprintf(
            stderr, "ERROR: only %d glyphs are supported, "\
            "but the font has %ld\n", MAX_N_FONT_GLYPHS, n_glyphs
        );
        goto fail;
    }

    for (size_t c = 0; c < n_glyphs; ++c) {
        GUIGlyph* glyph = &font->glyphs[c];
        memcpy(&glyph->id, &block[0 + c * 20], 4);
        memcpy(&glyph->x, &block[4 + c * 20], 2);
        memcpy(&glyph->y, &block[6 + c * 20], 2);
        memcpy(&glyph->width, &block[8 + c * 20], 2);
        memcpy(&glyph->height, &block[10 + c * 20], 2);
        memcpy(&glyph->xoffset, &block[12 + c * 20], 2);
        memcpy(&glyph->yoffset, &block[14 + c * 20], 2);
    }

    free(bytes);

    return sprite_create_from_file(&font->sprite, sprite_file_path);
fail:
    fprintf(stderr, "ERROR: bad font file\n");
    free(bytes);
    return false;
}

void gui_font_pack(GUIFont* gui_font, float dst[]) {
    // for (size_t i = 0; i < gui_font->n_glyphs; ++i) {
    // }
}

