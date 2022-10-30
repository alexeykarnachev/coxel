typedef struct Texture {
    GLuint tex;
    size_t target;
    size_t level;
    size_t width;
    size_t height;
    size_t internal_format;
    size_t format;
    size_t type;
} Texture;


bool texture_create_1d(
    Texture* texture,
    void* data,
    size_t level,
    size_t width,
    size_t internal_format,
    size_t format,
    size_t type
) {
    glCreateTextures(GL_TEXTURE_1D, 1, &texture->tex);
    glBindTexture(GL_TEXTURE_1D, texture->tex);

    // TODO: check opengl errors and return false in the case of any fails.
    glTexImage1D(GL_TEXTURE_1D, level, internal_format, width, 0, format, type, data);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_1D, 0);

    texture->target = GL_TEXTURE_1D;
    texture->level = level;
    texture->width = width;
    texture->height = 1;
    texture->internal_format = internal_format;
    texture->format = format;
    texture->type = type;

    return true;
}

