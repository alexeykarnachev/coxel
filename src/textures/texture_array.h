typedef struct TextureArray {
    GLuint tex;
    size_t width;
    size_t height;
    size_t n_levels;
    size_t n_layers;
    size_t n_channels;
} TextureArray;


bool texture_array_create(
    TextureArray* t,
    size_t width,
    size_t height,
    size_t n_levels,
    size_t n_layers,
    size_t n_channels
) {
    GLuint format;
    if (n_channels == 3) {
        format = GL_RGB;
    } else if (n_channels == 4) {
        format = GL_RGBA;
    } else {
        fprintf(stderr, "ERROR: can't create a texture. Number of channels must be 3 or 4");
        return false;
    }

    glGenTextures(1, &t->tex);
    glBindTexture(GL_TEXTURE_2D_ARRAY, t->tex);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, n_levels, GL_RGBA, width, height, n_layers);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // TODO: Add opengl errors checking

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    t->width = width;
    t->height = height;
    t->n_levels = n_levels;
    t->n_channels = n_channels;

    return true;
}

