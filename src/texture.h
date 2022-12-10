#pragma once
#include <glad/glad.h>
#include <stddef.h>


typedef struct Texture {
    GLuint tex;
} Texture;


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
);

