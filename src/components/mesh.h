typedef struct Mesh {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    size_t n_faces;
} Mesh;

Mesh _MESH_ARENA[MAX_N_MESHES];
size_t N_MESHES = 0;
size_t _CURRENT_MESH_ID = 0;


Mesh* mesh_get(size_t id) {
    return &_MESH_ARENA[id - MESH_START_ID];
}

int mesh_create(
    const unsigned char* faces,
    size_t faces_size,
    const float* vertices,
    size_t vertices_size
) {
    if (N_MESHES == MAX_N_MESHES) {
        fprintf(stderr, "ERROR: max number of meshes is reached. Mesh won't be created");
        return -1;
    }
    size_t id = MESH_START_ID + (N_MESHES++);
    Mesh* mesh = mesh_get(id);

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

    return id;
}

int mesh_create_icosahedron() {
    return mesh_create(
        ICOSAHEDRON_FACES, sizeof(ICOSAHEDRON_FACES), ICOSAHEDRON_VERTS, sizeof(ICOSAHEDRON_VERTS));
}

int mesh_create_plane() {
    return mesh_create(PLANE_FACES, sizeof(PLANE_FACES), PLANE_VERTS, sizeof(PLANE_VERTS));
}

void mesh_draw(size_t mesh_id) {
    Mesh* mesh = mesh_get(mesh_id);
    if (mesh_id != _CURRENT_MESH_ID) {
        glBindVertexArray(mesh->vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    }

    glDrawElements(GL_TRIANGLES, mesh->n_faces, GL_UNSIGNED_BYTE, 0);
}
