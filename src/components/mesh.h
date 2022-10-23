# define _MESH_UBO_N_BYTES = 16;

typedef struct Mesh {
    size_t material_id;
    size_t transformation_id;
    size_t n_transformations;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    size_t n_faces;
} Mesh;

int _MESH_UBO = -1;
Mesh _MESH_ARENA[MAX_N_MESHES];
size_t _MESH_ARENA_IDX = 0;

void _mesh_pack(Mesh* mesh, float dst[]) {
    dst[0] = mesh->material_id;
    dst[1] = mesh->tranformation_id;
    dst[2] = mesh->n_transformations;
}

int mesh_create(
    size_t material_id;
    size_t tranformation_id;
    size_t n_transformations,
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

    mesh->material = material;
    mesh->transformation = transformation;
    mesh->n_faces = faces_size / sizeof(faces[0]);

    static float data[_MESH_UBO_N_BYTES / 4];
    _mesh_pack(mesh, data);

    if (_MESH_UBO == -1) {
        glGenBuffers(1, &_MESH_UBO);
        glBindBuffer(GL_UNIFORM_BUFFER, _MESH_UBO);
        glBufferData(
            GL_UNIFORM_BUFFER,
            _MESH_UBO_N_BYTES * MAX_N_MESHES + 16,
            NULL,
            GL_STATIC_DRAW
        );
    }

    glBindBuffer(GL_UNIFORM_BUFFER, _MESH_UBO);
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        _MESH_ARENA_IDX * _MESH_UBO_N_BYTES,
        _MESH_UBO_N_BYTES,
        data
    );
    _MESH_ARENA_IDX += 1;

    glBufferSubData(
        GL_UNIFORM_BUFFER,
        MAX_N_MESHES * _MESH_UBO_N_BYTES,
        16,
        &_MESH_ARENA_IDX
    );

    glBindBufferBase(GL_UNIFORM_BUFFER, MESH_BINDING_IDX, _MESH_UBO);

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

    return _MESH_ARENA_IDX - 1;
}

int mesh_create_icosahedron(
    size_t material_id,
    size_t tranformation_id,
    size_t n_transformations
) {
    return mesh_create(
        material_id,
        transformation_id,
        n_transformations,
        ICOSAHEDRON_FACES,
        sizeof(ICOSAHEDRON_FACES),
        ICOSAHEDRON_VERTS,
        sizeof(ICOSAHEDRON_VERTS)
    );
}

int mesh_create_plane(
    size_t material_id,
    size_t tranformation_id,
    size_t n_transformations
) {
    return mesh_create(
        material_id,
        transformation_id,
        n_transformations,
        PLANE_FACES,
        sizeof(PLANE_FACES),
        PLANE_VERTS,
        sizeof(PLANE_VERTS)
    );
}

void mesh_bind(size_t mesh_id) {
    Mesh* mesh = &_MESH_ARENA[mesh_id];
    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
}
