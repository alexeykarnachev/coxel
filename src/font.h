static size_t GUI_FONT_TEXTURE_WIDTH = 1440;
static size_t GUI_FONT_TEXTURE_HEIGHT = 34;


char* font_load_raster() {
    size_t n_bytes;
    char* data = read_cstr_file("./assets/fonts/font.bin", "rb", &n_bytes);
    return data;
}
