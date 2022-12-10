#pragma once
#include "texture.h"
#include "buffers/gbuffer.h"
#include "buffers/overlay_buffer.h"


typedef struct Renderer {
    size_t viewport_width;
    size_t viewport_height;

    GBuffer gbuffer;
    OverlayBuffer overlay_buffer;
    Texture gui_font_texture;
} Renderer;

int renderer_create(
    Renderer* renderer,
    size_t gbuffer_width,
    size_t gbuffer_height,
    size_t overlay_buffer_width,
    size_t overlay_buffer_height
);
int renderer_update(Renderer* renderer);
