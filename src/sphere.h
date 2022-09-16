typedef struct Sphere {
    GLuint program;
    GLuint vbo;
    GLuint vao;
    GLuint ebo;

    GLuint a_pos;
    GLuint u_mv;
    GLuint u_proj;
    GLuint u_tess_lvl_inner;
    GLuint u_tess_lvl_outer;
} Sphere;

void sphere_draw(
        Sphere* sphere,
        Camera* camera,
        float* mv,
        float* proj,
        float tess_lvl_inner,
        float tess_lvl_outer
) {
    glUseProgram(sphere->program);
    
    glUniformMatrix4fv(sphere->u_mv, 1, GL_TRUE, mv);
    glUniformMatrix4fv(sphere->u_proj, 1, GL_TRUE, proj);
    glUniform1f(sphere->u_tess_lvl_inner, tess_lvl_inner);
    glUniform1f(sphere->u_tess_lvl_outer, tess_lvl_outer);

    glBindVertexArray(sphere->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->ebo);
    glDrawElements(GL_PATCHES, sizeof(ICOSAHEDRON_FACES) / sizeof(ICOSAHEDRON_FACES[0]),  GL_UNSIGNED_BYTE, 0);
}

bool sphere_create(Sphere* sphere) {
    GLuint program = glCreateProgram();
    bool is_linked = link_program_files(
        "./shaders/sphere/sphere.vert",
        "./shaders/sphere/sphere.tesc",
        "./shaders/sphere/sphere.tese",
        "./shaders/sphere/sphere.frag",
        program
    );
    if (!is_linked) {
        fprintf(stderr, "ERROR: failed to link sphere program files\n");
        return false;
    }

    bool ok = true;
    ok &= get_attrib_location(&(sphere->a_pos), program, "a_pos");
    ok &= get_uniform_location(&(sphere->u_mv), program, "u_mv");
    ok &= get_uniform_location(&(sphere->u_proj), program, "u_proj");
    ok &= get_uniform_location(&(sphere->u_tess_lvl_inner), program, "u_tess_lvl_inner");
    ok &= get_uniform_location(&(sphere->u_tess_lvl_outer), program, "u_tess_lvl_outer");
    if (!ok) {
        fprintf(stderr, "ERROR: failed to find some attribute or uniform locations in the shader program\n");
        return false;
    }

    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ICOSAHEDRON_VERTS), ICOSAHEDRON_VERTS, GL_STATIC_DRAW);

    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(sphere->a_pos);
    glVertexAttribPointer(sphere->a_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    GLuint ebo;
    glCreateBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ICOSAHEDRON_FACES), ICOSAHEDRON_FACES, GL_STATIC_DRAW);

    sphere->program = program;
    sphere->vbo = vbo;
    sphere->vao = vao;
    sphere->ebo = ebo;

    return true;
}

