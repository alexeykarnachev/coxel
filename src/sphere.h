typedef struct Sphere {
    Vec3 rotation;
    Vec3 translation;
    Vec3 scale;

    GLuint program;
    GLuint vbo;
    GLuint vao;
    GLuint ebo;

    GLuint a_pos;
    GLuint u_model;
    GLuint u_view;
    GLuint u_proj;
    GLuint u_tess_lvl_inner;
    GLuint u_tess_lvl_outer;
} Sphere;

typedef struct Planet {
    Sphere sphere;

    GLuint u_center_pos;
    GLuint u_light_pos;
    GLuint u_diffuse_color;
    GLuint u_ambient_color;
    GLuint u_light_color;
} Planet;

typedef struct Sun {
    Sphere sphere;

    GLuint u_center_pos;
    GLuint u_color;
} Sun;

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

    Mat3 rotation_mat = mat3_rotation(
        sphere->rotation.data[0],
        sphere->rotation.data[1],
        sphere->rotation.data[2]
    );
    Mat3 rs_mat = mat3_mat3_mul(&scale_mat, &rotation_mat);
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

    Mat4 model = sphere_get_model_mat(sphere);
    Mat4 view = cam_get_view_mat(camera);
    Mat4 proj = cam_get_perspective_projection_mat(camera);
    
    glUniformMatrix4fv(sphere->u_model, 1, GL_TRUE, model.data);
    glUniformMatrix4fv(sphere->u_view, 1, GL_TRUE, view.data);
    glUniformMatrix4fv(sphere->u_proj, 1, GL_TRUE, proj.data);
    glUniform1f(sphere->u_tess_lvl_inner, 32.0f);
    glUniform1f(sphere->u_tess_lvl_outer, 32.0f);

    glBindVertexArray(sphere->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->ebo);
    glDrawElements(GL_PATCHES, sizeof(ICOSAHEDRON_FACES) / sizeof(ICOSAHEDRON_FACES[0]),  GL_UNSIGNED_BYTE, 0);
}

void sphere_draw_planet(
        Planet* planet,
        Camera* camera,
        Vec3* light_pos,
        Vec3* diffuse_color,
        Vec3* ambient_color,
        Vec3* light_color
) {
    glUseProgram(planet->sphere.program);

    glUniform3fv(planet->u_center_pos, 1, planet->sphere.translation.data);
    glUniform3fv(planet->u_light_pos, 1, light_pos->data);
    glUniform3fv(planet->u_diffuse_color, 1, diffuse_color->data);
    glUniform3fv(planet->u_ambient_color, 1, ambient_color->data);
    glUniform3fv(planet->u_light_color, 1, light_color->data);

    sphere_draw(&planet->sphere, camera);
}

void sphere_draw_sun(
        Sun* sun,
        Camera* camera,
        Vec3* color
) {
    glUseProgram(sun->sphere.program);

    glUniform3fv(sun->u_center_pos, 1, sun->sphere.translation.data);
    glUniform3fv(sun->u_color, 1, color->data);
    sphere_draw(&sun->sphere, camera);
}

bool sphere_create(
    Sphere* sphere,
    const char* tese_file_path,
    const char* frag_file_path
) {
    memset(sphere, 0, sizeof(*sphere));
    GLuint program = glCreateProgram();

    int n_deps = 1;
    const char* random_file_path = "./shaders/common/random.glsl"; 
    const char* deps_file_paths[1] = {random_file_path};

    bool is_linked = shader_link_program(
        "./shaders/sphere/sphere.vert",
        "./shaders/sphere/sphere.tesc",
        tese_file_path,
        frag_file_path,
        program,
        n_deps,
        deps_file_paths
    );
    if (!is_linked) {
        fprintf(stderr, "ERROR: failed to link sphere program\n");
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
    ok &= shader_get_attrib_location(&(sphere->a_pos), program, "a_pos");
    ok &= shader_get_uniform_location(&(sphere->u_model), program, "u_model");
    ok &= shader_get_uniform_location(&(sphere->u_view), program, "u_view");
    ok &= shader_get_uniform_location(&(sphere->u_proj), program, "u_proj");
    ok &= shader_get_uniform_location(&(sphere->u_tess_lvl_inner), program, "u_tess_lvl_inner");
    ok &= shader_get_uniform_location(&(sphere->u_tess_lvl_outer), program, "u_tess_lvl_outer");
    if (!ok) {
        fprintf(stderr, "ERROR: failed to find some attribute or uniform locations in the sphere program\n");
        return false;
    }

    return true;
}

bool sphere_create_planet(Planet* planet) {
    bool ok = sphere_create(&planet->sphere, "./shaders/sphere/planet.tese", "./shaders/sphere/planet.frag");

    ok &= shader_get_uniform_location(&(planet->u_center_pos), planet->sphere.program, "u_center_pos");
    ok &= shader_get_uniform_location(&(planet->u_light_pos), planet->sphere.program, "u_light_pos");
    ok &= shader_get_uniform_location(&(planet->u_diffuse_color), planet->sphere.program, "u_diffuse_color");
    ok &= shader_get_uniform_location(&(planet->u_ambient_color), planet->sphere.program, "u_ambient_color");
    ok &= shader_get_uniform_location(&(planet->u_light_color), planet->sphere.program, "u_light_color");
    if (!ok) {
        fprintf(stderr, "ERROR: failed to find some attribute or uniform locations in the planet program\n");
        return false;
    }

    return true;
}

bool sphere_create_sun(Sun* sun) {
    bool ok = sphere_create(&sun->sphere, "./shaders/sphere/sun.tese", "./shaders/sphere/sun.frag");

    ok &= shader_get_uniform_location(&(sun->u_center_pos), sun->sphere.program, "u_center_pos");
    ok &= shader_get_uniform_location(&(sun->u_color), sun->sphere.program, "u_color");
    if (!ok) {
        fprintf(stderr, "ERROR: failed to find some attribute or uniform locations in the sun program\n");
        return false;
    }

    return true;
}
