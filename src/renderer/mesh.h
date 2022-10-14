void mesh_create(
    Mesh* mesh,
    Material* material,
    Transformation* transformation,
    GLuint program,
    const unsigned char* faces,
    size_t faces_size,
    const float* vertices,
    size_t vertices_size
);

void mesh_create_icosahedron(
    Mesh* mesh,
    Material* material,
    Transformation* transformation,
    GLuint program
);

void mesh_create_plane(
    Mesh* mesh,
    Material* material,
    Transformation* transformation,
    GLuint program
);


typedef struct Mesh {
    Material* material;
    Transformation* transformation;
    GLuint program;
    GLuint vao;
    Gluint vbo;
    GLuint ebo;
    size_t n_elements;
} Mesh;

void mesh_create(
    Mesh* mesh,
    Material* material,
    Transformation* transformation,
    GLuint program,
    const unsigned char* faces,
    size_t faces_size,
    const float* vertices,
    size_t vertices_size
) {
    clear_struct(mesh);

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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Transformation transformation;
    transformation_create(&transformation, scale, rotation, translation);

    mesh->material = material;
    mesh->transformation = transformation;
    mesh->program = program;
    mesh->vbo = vbo;
    mesh->vao = vao;
    mesh->ebo = ebo;
    mesh->n_elements = faces_size / sizeof(faces[0]);
}

void mesh_create_icosahedron(
    Mesh* mesh,
    Material* material,
    Transformation* transformation,
    GLuint program
) {
    mesh_create(
        mesh,
        material,
        transformation,
        program,
        ICOSAHEDRON_FACES,
        sizeof(ICOSAHEDRON_FACES),
        ICOSAHEDRON_VERTS,
        sizeof(ICOSAHEDRON_VERTS)
    );
}

void mesh_create_plane(
    Mesh* mesh,
    Material* material,
    Transformation* transformation,
    GLuint program
) {
    mesh_create(
        mesh,
        material,
        transformation,
        program,
        PLANE_FACES,
        sizeof(PLANE_FACES),
        PLANE_VERTS,
        sizeof(PLANE_VERTS)
    );
}

static unsigned char ICOSAHEDRON_FACES[] = {
    0,  1,   2,
    0,  2,   3,
    0,  3,   4,
    0,  4,   5,
    0,  5,   1,
    7,  6,   11,
    8,  7,   11,
    9,  8,   11,
    10,  9,   11,
    6,  10,  11,
    6,  2,   1,
    7,  3,   2,
    8,  4,   3,
    9,  5,   4,
    10,  1,   5,
    6,  7,   2,
    7,  8,   3,
    8,  9,   4,
    9,  10,  5,
    10,  6,   1,
};

static float ICOSAHEDRON_VERTS[] = {
     0.000f,  0.000f,  1.000f,
     0.894f,  0.000f,  0.447f,
     0.276f,  0.851f,  0.447f,
    -0.724f,  0.526f,  0.447f,
    -0.724f, -0.526f,  0.447f,
     0.276f, -0.851f,  0.447f,
     0.724f,  0.526f, -0.447f,
    -0.276f,  0.851f, -0.447f,
    -0.894f,  0.000f, -0.447f,
    -0.276f, -0.851f, -0.447f,
     0.724f, -0.526f, -0.447f,
     0.000f,  0.000f, -1.000f
};

static unsigned char PLANE_FACES[] = {2, 1, 0, 3, 2, 0};

static float PLANE_VERTS[] = {
     -0.5f,  -0.5f,  0.0f,
     -0.5f,  0.5f,  0.0f,
     0.5f,  0.5f,  0.0f,
     0.5f,  -0.5f,  0.0f
};
