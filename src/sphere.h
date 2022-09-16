typedef struct Sphere {
    Vec3 rotation;
    Vec3 translation;
    Vec3 scale;

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

void sphere_scale(Sphere* sphere, float xd, float yd, float zd) {
    sphere->scale.data[0] += xd;
    sphere->scale.data[1] += yd;
    sphere->scale.data[2] += zd;
}

void sphere_translate(Sphere* sphere, float xd, float yd, float zd) {
    sphere->translation.data[0] += xd;
    sphere->translation.data[1] += yd;
    sphere->translation.data[2] += zd;
}

void sphere_rotate(Sphere* sphere, float xa, float ya, float za) {
    // TODO: mod by PI:
    sphere->rotation.data[0] += xa;
    sphere->rotation.data[1] += ya;
    sphere->rotation.data[2] += za;
}

static Mat4 sphere_get_model_mat(Sphere* sphere) {
    Mat3 scale_mat = {{
        sphere->scale.data[0], 0.0, 0.0,
        0.0, sphere->scale.data[1], 0.0,
        0.0, 0.0, sphere->scale.data[2]
    }};

    Mat3 rotation_mat = mat3_rotation(sphere->rotation.data[0], sphere->rotation.data[1], sphere->rotation.data[2]);
    Mat3 rs_mat = mat3_matmul(&scale_mat, &rotation_mat);
    Mat4 model_mat = {{
        rs_mat.data[0], rs_mat.data[1], rs_mat.data[2], sphere->translation.data[0],
        rs_mat.data[3], rs_mat.data[4], rs_mat.data[5], sphere->translation.data[1],
        rs_mat.data[6], rs_mat.data[7], rs_mat.data[8], sphere->translation.data[2],
        0.0f, 0.0f, 0.0f, 1.0f
    }};

    return model_mat;
}

void sphere_draw(Sphere* sphere, Camera* camera) {
    glUseProgram(sphere->program);

    Mat4 view = cam_get_view_mat(camera);
    Mat4 model = sphere_get_model_mat(sphere);
    Mat4 mv = mat4_matmul(&view, &model);

    Mat4 proj = cam_get_perspective_projection_mat(camera);
    
    glUniformMatrix4fv(sphere->u_mv, 1, GL_TRUE, mv.data);
    glUniformMatrix4fv(sphere->u_proj, 1, GL_TRUE, proj.data);
    glUniform1f(sphere->u_tess_lvl_inner, 8.0f);
    glUniform1f(sphere->u_tess_lvl_outer, 8.0f);

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
    sphere->rotation = vec3_zeros();
    sphere->translation = vec3_zeros();
    sphere->scale = vec3_ones();

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

    return true;
}
