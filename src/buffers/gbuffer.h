#pragma once
#include <glad/glad.h>

#include "../includes.h"
#include "../texture.h"

typedef struct GBuffer {
    size_t width;
    size_t height;
    GLuint fbo;
    GLuint rbo;
    Texture world_pos_texture;
    Texture world_norm_texture;
    Texture diffuse_texture;
    Texture specular_texture;
    Texture entity_id_texture;
} GBuffer;

int gbuffer_create(GBuffer* buffer, size_t width, size_t height);
