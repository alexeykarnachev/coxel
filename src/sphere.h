typedef struct SphereProgram {
    GLuint program;

    GLuint a_pos;
    GLuint u_model;
    GLuint u_view;
    GLuint u_proj;

    GLuint u_tess_lvl_inner;
    GLuint u_tess_lvl_outer;

    GLuint u_surface_noise_n_levels;
    GLuint u_surface_noise_freq_mult;
    GLuint u_surface_noise_ampl_mult;
    GLuint u_surface_noise_freq_init;
    GLuint u_surface_noise_mult;

    GLuint u_view_pos;
    GLuint u_light_pos;
    GLuint u_diffuse_color;
    GLuint u_light_color;
    GLuint u_ambient_weight;
    GLuint u_specular_power;

    GLuint u_calc_shadows;
    GLuint u_far;
    GLuint u_shadow_transforms[6];
} SphereProgram;

typedef struct Sphere {
    Vec3 rotation;
    Vec3 translation;
    Vec3 scale;

    SphereProgram color_program;
    SphereProgram shadow_program;
    GLuint vbo;
    GLuint vao;
    GLuint ebo;
} Sphere;

void sphere_draw(
    Sphere* sphere,
    Camera* camera,
    float tess_lvl_inner,
    float tess_lvl_outer,
    int surface_noise_n_levels,
    float surface_noise_freq_mult,
    float surface_noise_ampl_mult,
    float surface_noise_freq_init,
    float surface_noise_mult,
    Vec3* light_pos,
    Vec3* diffuse_color,
    Vec3* light_color,
    float ambient_weight,
    float specular_power,
    bool calc_shadows
) {
    SphereProgram program;
    if (!calc_shadows) {
        program = sphere->shadow_program;
        Mat4 shadow_proj = get_perspective_projection_mat(90.0, camera->near, camera->far, camera->aspect);
        Mat4 shadow_transforms[6];
        shadow_transforms[0] = get_view_mat(&vec3_pos_x, &vec3_neg_y, light_pos);
        shadow_transforms[1] = get_view_mat(&vec3_neg_x, &vec3_neg_y, light_pos);
        shadow_transforms[2] = get_view_mat(&vec3_pos_y, &vec3_pos_z, light_pos);
        shadow_transforms[3] = get_view_mat(&vec3_neg_y, &vec3_neg_z, light_pos);
        shadow_transforms[4] = get_view_mat(&vec3_pos_z, &vec3_neg_y, light_pos);
        shadow_transforms[5] = get_view_mat(&vec3_neg_z, &vec3_neg_y, light_pos);
        for (size_t i = 0; i < 6; ++i) {
            glUniformMatrix4fv(program.u_shadow_transforms[i], 1, GL_TRUE, shadow_transforms[i].data);
        }
    } else {
        program = sphere->color_program;
    }

    glUseProgram(program.program);

    Mat4 model = get_model_mat(sphere);
    Mat4 view = cam_get_view_mat(camera);
    Mat4 proj = cam_get_perspective_projection_mat(camera);
    
    glUniformMatrix4fv(program.u_model, 1, GL_TRUE, model.data);
    glUniformMatrix4fv(program.u_view, 1, GL_TRUE, view.data);
    glUniformMatrix4fv(program.u_proj, 1, GL_TRUE, proj.data);

    glUniform1f(program.u_tess_lvl_inner, tess_lvl_inner);
    glUniform1f(program.u_tess_lvl_outer, tess_lvl_outer);

    glUniform1i(program.u_surface_noise_n_levels, surface_noise_n_levels);
    glUniform1f(program.u_surface_noise_freq_mult, surface_noise_freq_mult);
    glUniform1f(program.u_surface_noise_ampl_mult, surface_noise_ampl_mult);
    glUniform1f(program.u_surface_noise_freq_init, surface_noise_freq_init);
    glUniform1f(program.u_surface_noise_mult, surface_noise_mult);

    glUniform3fv(program.u_view_pos, 1, camera->translation.data);
    glUniform3fv(program.u_light_pos, 1, light_pos->data);
    glUniform3fv(program.u_diffuse_color, 1, diffuse_color->data);
    glUniform3fv(program.u_light_color, 1, light_color->data);
    glUniform1f(program.u_ambient_weight, ambient_weight);
    glUniform1f(program.u_specular_power, specular_power);

    glUniform1i(program.u_calc_shadows, calc_shadows);
    glUniform1f(program.u_far, camera->far);

    glBindVertexArray(sphere->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->ebo);
    glDrawElements(GL_PATCHES, sizeof(ICOSAHEDRON_FACES) / sizeof(ICOSAHEDRON_FACES[0]),  GL_UNSIGNED_BYTE, 0);
}

bool sphere_shader_link_program(Sphere* sphere, bool calc_shadows) {
    GLuint gl_program = glCreateProgram();
    SphereProgram sphere_program;

    const char* deps[1] = {"./shaders/common/random.glsl"};
    const char* vert = "./shaders/sphere/sphere.vert"; 
    const char* tesc = "./shaders/sphere/sphere.tesc"; 
    const char* tese = "./shaders/sphere/sphere.tese"; 
    const char* frag = "./shaders/common/lighting.frag"; 
    const char* geom = !calc_shadows ? "./shaders/common/cube_shadows.geom" : NULL;
    if (!shader_link_program(vert, tesc, tese, geom, frag, gl_program, 1, deps)) {
        return false;
    }
    sphere_program.program = gl_program;

    if (
        !shader_get_attrib_location(&(sphere_program.a_pos), gl_program, "a_pos")
        || !shader_get_uniform_location(&(sphere_program.u_model), gl_program, "u_model")
        || !shader_get_uniform_location(&(sphere_program.u_view), gl_program, "u_view")
        || !shader_get_uniform_location(&(sphere_program.u_proj), gl_program, "u_proj")
        || !shader_get_uniform_location(&(sphere_program.u_tess_lvl_inner), gl_program, "u_tess_lvl_inner")
        || !shader_get_uniform_location(&(sphere_program.u_tess_lvl_outer), gl_program, "u_tess_lvl_outer")
        || !shader_get_uniform_location(&(sphere_program.u_surface_noise_n_levels), gl_program, "u_surface_noise_n_levels")
        || !shader_get_uniform_location(&(sphere_program.u_surface_noise_freq_mult), gl_program, "u_surface_noise_freq_mult")
        || !shader_get_uniform_location(&(sphere_program.u_surface_noise_ampl_mult), gl_program, "u_surface_noise_ampl_mult")
        || !shader_get_uniform_location(&(sphere_program.u_surface_noise_freq_init), gl_program, "u_surface_noise_freq_init")
        || !shader_get_uniform_location(&(sphere_program.u_surface_noise_mult), gl_program, "u_surface_noise_mult")
        || !shader_get_uniform_location(&(sphere_program.u_view_pos), gl_program, "u_view_pos")
        || !shader_get_uniform_location(&(sphere_program.u_light_pos), gl_program, "u_light_pos")
        || !shader_get_uniform_location(&(sphere_program.u_diffuse_color), gl_program, "u_diffuse_color")
        || !shader_get_uniform_location(&(sphere_program.u_light_color), gl_program, "u_light_color")
        || !shader_get_uniform_location(&(sphere_program.u_ambient_weight), gl_program, "u_ambient_weight")
        || !shader_get_uniform_location(&(sphere_program.u_specular_power), gl_program, "u_specular_power")
        || !shader_get_uniform_location(&(sphere_program.u_calc_shadows), gl_program, "u_calc_shadows")
        || !shader_get_uniform_location(&(sphere_program.u_far), gl_program, "u_far")
    ) {
        return false;
    }

    if (!calc_shadows) {
        bool ok = true;
        ok &= shader_get_uniform_location(&(sphere_program.u_shadow_transforms[0]), gl_program, "u_shadow_transforms[0]");
        ok &= shader_get_uniform_location(&(sphere_program.u_shadow_transforms[1]), gl_program, "u_shadow_transforms[1]");
        ok &= shader_get_uniform_location(&(sphere_program.u_shadow_transforms[2]), gl_program, "u_shadow_transforms[2]");
        ok &= shader_get_uniform_location(&(sphere_program.u_shadow_transforms[3]), gl_program, "u_shadow_transforms[3]");
        ok &= shader_get_uniform_location(&(sphere_program.u_shadow_transforms[4]), gl_program, "u_shadow_transforms[4]");
        ok &= shader_get_uniform_location(&(sphere_program.u_shadow_transforms[5]), gl_program, "u_shadow_transforms[5]");
        if (!ok) {
            return false;
        }
        sphere->shadow_program = sphere_program;
    } else {
        sphere->color_program = sphere_program;
    }

    return true;
}

bool sphere_create(Sphere* sphere) {
    memset(sphere, 0, sizeof(*sphere));

    if (!sphere_shader_link_program(sphere, false) || !sphere_shader_link_program(sphere, true)) {
        return false;
    }

    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(ICOSAHEDRON_VERTS), ICOSAHEDRON_VERTS, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ICOSAHEDRON_FACES), ICOSAHEDRON_FACES, GL_STATIC_DRAW);

    // TODO: remove this glUseProgram?
    glUseProgram(sphere->color_program.program);
    glEnableVertexAttribArray(sphere->color_program.a_pos);
    glVertexAttribPointer(sphere->color_program.a_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    // TODO: remove this glUseProgram?
    glUseProgram(sphere->shadow_program.program);
    glEnableVertexAttribArray(sphere->shadow_program.a_pos);
    glVertexAttribPointer(sphere->shadow_program.a_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    sphere->vbo = vbo;
    sphere->vao = vao;
    sphere->ebo = ebo;
    sphere->rotation = vec3_zeros();
    sphere->translation = vec3_zeros();
    sphere->scale = vec3_ones();

    return true;
}

