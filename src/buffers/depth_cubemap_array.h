#pragma once
#include <glad/glad.h>
#include <stddef.h>


typedef struct DepthCubemapArray {
    GLuint fbo;
    GLuint tex;
    size_t size;
    size_t n_layers;
} DepthCubemapArray;


int depth_cubemap_array_create(
    DepthCubemapArray* d,
    size_t size,
    size_t n_layers
);
