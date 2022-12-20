#pragma once
#include <glad/glad.h>

#include "../texture.h"

typedef struct OverlayBuffer {
    size_t width;
    size_t height;
    GLuint fbo;
    GLuint rbo;
    Texture outline_texture;
    Texture gui_text_texture;
    Texture gui_rect_texture;
    Texture entity_id_texture;
} OverlayBuffer;

int overlay_buffer_create(
    OverlayBuffer* buffer, size_t width, size_t height
);
int overlay_buffer_get_entity_id_at(OverlayBuffer* buffer, int x, int y);
int overlay_buffer_get_entity_id_at_cursor(OverlayBuffer* buffer);
