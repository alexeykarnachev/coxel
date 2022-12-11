#include "texture.h"

int texture_create_2d(
    Texture* texture,
    void* data,
    size_t level,
    size_t width,
    size_t height,
    size_t internal_format,
    size_t format,
    size_t type,
    int filter
) {
    glCreateTextures(GL_TEXTURE_2D, 1, &texture->tex);
    glBindTexture(GL_TEXTURE_2D, texture->tex);

    // TODO: check opengl errors and return false in the case of any fails.
    glTexImage2D(
        GL_TEXTURE_2D,
        level,
        internal_format,
        width,
        height,
        0,
        format,
        type,
        data
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glBindTexture(GL_TEXTURE_2D, 0);

    return 1;
}
