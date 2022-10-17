typedef struct Mesh {
    Material material;
    Transformation transformation;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    size_t n_elements;
} Mesh;

void mesh_create(
    Mesh* mesh,
    Material material,
    Transformation transformation,
    const unsigned char* faces,
    size_t faces_size,
    const float* vertices,
    size_t vertices_size
);

void mesh_create_icosahedron(
    Mesh* mesh,
    Material material,
    Transformation transformation
);

void mesh_create_plane(
    Mesh* mesh,
    Material material,
    Transformation transformation
);

void mesh_create(
    Mesh* mesh,
    Material material,
    Transformation transformation,
    const unsigned char* faces,
    size_t faces_size,
    const float* vertices,
    size_t vertices_size
) {
    clear_struct(mesh);

    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glCreateBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

    glCreateBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces_size, faces, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    mesh->material = material;
    mesh->transformation = transformation;
    mesh->vbo = vbo;
    mesh->vao = vao;
    mesh->ebo = ebo;
    mesh->n_elements = faces_size / sizeof(faces[0]);
}

void mesh_create_icosahedron(
    Mesh* mesh,
    Material material,
    Transformation transformation
) {
    mesh_create(
        mesh,
        material,
        transformation,
        ICOSAHEDRON_FACES,
        sizeof(ICOSAHEDRON_FACES),
        ICOSAHEDRON_VERTS,
        sizeof(ICOSAHEDRON_VERTS)
    );
}

void mesh_create_plane(
    Mesh* mesh,
    Material material,
    Transformation transformation
) {
    mesh_create(
        mesh,
        material,
        transformation,
        PLANE_FACES,
        sizeof(PLANE_FACES),
        PLANE_VERTS,
        sizeof(PLANE_VERTS)
    );
}

void mesh_rotate(Mesh* mesh, float dx, float dy, float dz) {
    transformation_rotate(&mesh->transformation, dx, dy, dz);
}

void mesh_translate(Mesh* mesh, float dx, float dy, float dz) {
    transformation_translate(&mesh->transformation, dx, dy, dz);
}

void mesh_scale(Mesh* mesh, float dx, float dy, float dz) {
    transformation_scale(&mesh->transformation, dx, dy, dz);
}

void mesh_bind(Mesh* mesh) {
    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
}
