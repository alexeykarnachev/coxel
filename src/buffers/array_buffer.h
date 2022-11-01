typedef struct ArrayBuffer {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    size_t n_faces;
} ArrayBuffer;


bool array_buffer_create(
    ArrayBuffer* buffer,
    const uint32_t* faces,
    size_t faces_size,
    const float* vertices,
    size_t vertices_size,
    GLenum usage
) {
    buffer->n_faces = faces_size / sizeof(faces[0]);

    glCreateVertexArrays(1, &buffer->vao);
    glBindVertexArray(buffer->vao);

    glCreateBuffers(1, &buffer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, usage);

    glCreateBuffers(1, &buffer->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces_size, faces, usage);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

bool array_buffer_create_icosahedron(ArrayBuffer* buffer) {
    return array_buffer_create(
        buffer, ICOSAHEDRON_FACES, sizeof(ICOSAHEDRON_FACES),
        ICOSAHEDRON_VERTS, sizeof(ICOSAHEDRON_VERTS), GL_STATIC_DRAW
    );
}

bool array_buffer_create_plane(ArrayBuffer* buffer) {
    return array_buffer_create(
        buffer, PLANE_FACES, sizeof(PLANE_FACES),
        PLANE_VERTS, sizeof(PLANE_VERTS), GL_STATIC_DRAW
    );
}

void array_buffer_bind(ArrayBuffer* buffer) {
    glBindVertexArray(buffer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ebo);
}

