#pragma once
#include <glad/glad.h>

#include "../texture.h"

typedef struct OverlayBuffer {
    size_t width;
    size_t height;
    GLuint fbo;
    Texture outline_texture;
    Texture gui_text_texture;
    Texture gui_rect_texture;
} OverlayBuffer;

int overlay_buffer_create(
    OverlayBuffer* buffer, size_t width, size_t height
);
