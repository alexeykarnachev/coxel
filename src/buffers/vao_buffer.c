#include "vao_buffer.h"
#include "../assets/obj.h"


int vao_buffer_create(
    VAOBuffer* buffer,
    const float* vp,
    size_t vp_size,
    const float* vn,
    size_t vn_size,
    const uint32_t* f,
    size_t f_size,
    GLenum usage
) {
    buffer->n_f = f_size / sizeof(f[0]);

    glCreateVertexArrays(1, &buffer->vao);
    glBindVertexArray(buffer->vao);

    glCreateBuffers(1, &buffer->vp_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vp_vbo);
    glBufferData(GL_ARRAY_BUFFER, vp_size, vp, usage);

    glCreateBuffers(1, &buffer->vn_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vn_vbo);
    glBufferData(GL_ARRAY_BUFFER, vn_size, vn, usage);

    glCreateBuffers(1, &buffer->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, f_size, f, usage);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return 1;
}

int vao_buffer_create_from_obj(VAOBuffer* buffer, char* file_path) {
    float* vp;
    size_t vp_size;
    float* vn;
    size_t vn_size;
    uint32_t* f;
    size_t f_size;
    load_obj(
        file_path,
        &vp, &vp_size,
        &vn, &vn_size,
        &f, &f_size
    );
    int res = vao_buffer_create(
        buffer, vp, vp_size, vn, vn_size, f, f_size, GL_STATIC_DRAW);

    return res;
}


