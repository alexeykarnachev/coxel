typedef struct Texture {
    GLuint tex;
    size_t width;
    size_t height;
    size_t n_levels;
    size_t n_channels;
} Texture;


bool texture_create(
    Texture* t,
    size_t width,
    size_t height,
    size_t n_levels,
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
    glBindTexture(GL_TEXTURE_2D, t->tex);
    glTextureStorage2D(t->tex, n_levels, GL_RGBA, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // TODO: Add opengl errors checking

    glBindTexture(GL_TEXTURE_2D, 0);

    t->width = width;
    t->height = height;
    t->n_levels = n_levels;

    return true;
}

bool texture_set_sprite(Texture* texture, Sprite* sprite, size_t level) {
    GLuint format;
    if (texture->n_channels != sprite->n_channels) {
        fprintf(
            stderr, "ERROR: can't load sprite into a texture. "\
            "Sprite and texture have different number of channels"
        );
        return false;
    } else if (sprite->n_channels == 3) {
        format = GL_RGB;
    } else if (sprite->n_channels == 4) {
        format = GL_RGBA;
    } else {
        fprintf(stderr, "ERROR: can't load sprite into a texture. Number of channels must be 3 or 4");
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, texture->tex);
    glTexImage2D(
        GL_TEXTURE_2D,
        level,
        format,
        sprite->width,
        sprite->height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        sprite->data
    );
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}
