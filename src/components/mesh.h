typedef struct Mesh {
    const uint32_t* faces;
    size_t faces_size;
    const float* vertices;
    size_t vertices_size;
} Mesh;


Mesh mesh_create(
    const uint32_t* faces,
    size_t faces_size,
    const float* vertices,
    size_t vertices_size
) {
    Mesh mesh;
    mesh.faces = faces;
    mesh.faces_size = faces_size;
    mesh.vertices = vertices;
    mesh.vertices_size = vertices_size;
    
    return mesh;
}

Mesh mesh_create_icosahedron() {
    return mesh_create(
        ICOSAHEDRON_FACES, sizeof(ICOSAHEDRON_FACES), ICOSAHEDRON_VERTS, sizeof(ICOSAHEDRON_VERTS));
}

Mesh mesh_create_plane() {
    return mesh_create(PLANE_FACES, sizeof(PLANE_FACES), PLANE_VERTS, sizeof(PLANE_VERTS));
}

