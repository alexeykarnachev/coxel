typedef struct Model {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    size_t n_elements;
} Model;

bool model_create(
    Model* model,
    const unsigned char* faces,
    size_t faces_size,
    const float* vertices,
    size_t vertices_size
);

void model_draw_patches(Model* model, size_t n_vertices);

bool model_create(
    Model* model,
    const unsigned char* faces,
    size_t faces_size,
    const float* vertices,
    size_t vertices_size
) {
    memset(model, 0, sizeof(*model));

    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    glCreateBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glCreateBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces_size, faces, GL_STATIC_DRAW);

    model->vbo = vbo;
    model->vao = vao;
    model->ebo = ebo;
    model->n_elements = faces_size / sizeof(faces[0]);

    return true;
}

void model_draw_patches(Model* model, size_t n_vertices) {
    glPatchParameteri(GL_PATCH_VERTICES, n_vertices);
    glBindVertexArray(model->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
    glDrawElements(GL_PATCHES, model->n_elements, GL_UNSIGNED_BYTE, 0);
}

