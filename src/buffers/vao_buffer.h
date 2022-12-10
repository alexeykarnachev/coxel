#pragma once
#include <stddef.h>
#include <glad/glad.h>


typedef struct VAOBuffer {
    GLuint vao;
    GLuint ebo;
    GLuint vp_vbo;
    GLuint vn_vbo;
    size_t n_f;
} VAOBuffer;

int vao_buffer_create(
    VAOBuffer* buffer,
    const float* vp,
    size_t vp_size,
    const float* vn,
    size_t vn_size,
    const uint32_t* f,
    size_t f_size,
    GLenum usage
);
int vao_buffer_create_from_obj(VAOBuffer* buffer, char* file_path);
