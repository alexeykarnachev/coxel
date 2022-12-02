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

    // TODO: Remove after tests:
    // printf("vp: ");
    // for (size_t i = 0; i < vp_size / sizeof(float); ++i) {
    //     if (i % 3 == 0) {
    //         printf("\n");
    //     }
    //     printf("%f ", vp[i]);
    // }
    // printf("\n");

    // printf("vn: ");
    // for (size_t i = 0; i < vn_size / sizeof(float); ++i) {
    //     if (i % 3 == 0) {
    //         printf("\n");
    //     }
    //     printf("%f ", vn[i]);
    // }
    // printf("\n");

    // printf("f: ");
    // for (size_t i = 0; i < f_size / sizeof(int32_t); ++i) {
    //     if (i % 3 == 0) {
    //         printf("\n");
    //     }
    //     printf("%d ", f[i]);
    // }
    // printf("\n");

    return res;
}

