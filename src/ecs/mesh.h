typedef struct Mesh {
    VAOBuffer vao_buffer;
} Mesh;

Mesh _MESHES_ARENA[1 << 14];
size_t _N_MESHES = 0;

VAOBuffer _ICOSAHEDRON_VAO_BUFFER;
VAOBuffer _PLANE_VAO_BUFFER;


Mesh* mesh_create(VAOBuffer vao_buffer) {
    Mesh* m = &_MESHES_ARENA[_N_MESHES++];

    m->vao_buffer = vao_buffer;

    return m;
}

Mesh* mesh_create_icosahedron() {
    if (_ICOSAHEDRON_VAO_BUFFER.vao == 0) {
        vao_buffer_create(
            &_ICOSAHEDRON_VAO_BUFFER, ICOSAHEDRON_FACES,
            sizeof(ICOSAHEDRON_FACES), ICOSAHEDRON_VERTS,
            sizeof(ICOSAHEDRON_VERTS), GL_STATIC_DRAW
        );
    }
    return mesh_create(_ICOSAHEDRON_VAO_BUFFER);
}

Mesh* mesh_create_plane() {
    if (_PLANE_VAO_BUFFER.vao == 0) {
        vao_buffer_create(
            &_PLANE_VAO_BUFFER, PLANE_FACES,
            sizeof(PLANE_FACES), PLANE_VERTS,
            sizeof(PLANE_VERTS), GL_STATIC_DRAW
        );
    }
    return mesh_create(_PLANE_VAO_BUFFER);
}

