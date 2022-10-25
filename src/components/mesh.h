typedef struct Mesh {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    size_t n_faces;
} Mesh;

Mesh _MESH_ARENA[MAX_N_MESHES];
size_t _MESH_ARENA_IDX = 0;
size_t _MESH_N_FACES = 0;


int mesh_create(
    const unsigned char* faces,
    size_t faces_size,
    const float* vertices,
    size_t vertices_size
) {
    if (_MESH_ARENA_IDX == MAX_N_MESHES) {
        fprintf(stderr, "ERROR: max number of meshes is reached. Mesh won't be created");
        return -1;
    }
    Mesh* mesh = &_MESH_ARENA[_MESH_ARENA_IDX];

    mesh->n_faces = faces_size / sizeof(faces[0]);

    glCreateVertexArrays(1, &mesh->vao);
    glBindVertexArray(mesh->vao);

    glCreateBuffers(1, &mesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

    glCreateBuffers(1, &mesh->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces_size, faces, GL_STATIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return _MESH_ARENA_IDX++;
}

int mesh_create_icosahedron() {
    return mesh_create(
        ICOSAHEDRON_FACES, sizeof(ICOSAHEDRON_FACES), ICOSAHEDRON_VERTS, sizeof(ICOSAHEDRON_VERTS));
}

int mesh_create_plane() {
    return mesh_create(PLANE_FACES, sizeof(PLANE_FACES), PLANE_VERTS, sizeof(PLANE_VERTS));
}

void mesh_bind(size_t mesh_id) {
    Mesh* mesh = &_MESH_ARENA[mesh_id];
    _MESH_N_FACES = mesh->n_faces;

    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
}

void mesh_draw() {
    glDrawElements(GL_TRIANGLES, _MESH_N_FACES, GL_UNSIGNED_BYTE, 0);
}
