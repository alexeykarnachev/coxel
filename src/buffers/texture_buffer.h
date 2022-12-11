#pragma once
#include "../texture.h"

#include <glad/glad.h>

typedef struct TextureBuffer {
    GLuint fbo;
    GLuint rbo;
    Texture texture;
    size_t width;
    size_t height;
} TextureBuffer;

int texture_buffer_create(
    TextureBuffer* buffer,
    void* data,
    size_t width,
    size_t height,
    size_t internal_format,
    size_t format,
    size_t type
);
