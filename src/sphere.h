typedef struct Sphere {
    Vec3 rotation;
    Vec3 translation;
    Vec3 scale;

    GLuint color_program;
    GLuint shadow_program;
    GLuint vbo;
    GLuint vao;
    GLuint ebo;

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

    GLuint u_with_shadows;
    GLuint u_far;
    GLuint u_shadow_transforms[6];
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
    bool with_shadows
) {
    if (with_shadows) {
        glUseProgram(sphere->shadow_program);
        Mat4 shadow_proj = get_perspective_projection_mat(90.0, camera->near, camera->far, camera->aspect);
        Mat4 shadow_transforms[6];
        shadow_transforms[0] = get_view_mat(&vec3_pos_x, &vec3_neg_y, light_pos);
        shadow_transforms[1] = get_view_mat(&vec3_neg_x, &vec3_neg_y, light_pos);
        shadow_transforms[2] = get_view_mat(&vec3_pos_y, &vec3_pos_z, light_pos);
        shadow_transforms[3] = get_view_mat(&vec3_neg_y, &vec3_neg_z, light_pos);
        shadow_transforms[4] = get_view_mat(&vec3_pos_z, &vec3_neg_y, light_pos);
        shadow_transforms[5] = get_view_mat(&vec3_neg_z, &vec3_neg_y, light_pos);

        for (size_t i = 0; i < 6; ++i) {
            glUniformMatrix4fv(sphere->u_model, 1, GL_TRUE, shadow_transforms[i]);
        }


    } else {
        glUseProgram(sphere->color_program);
    }

    Mat4 model = sphere_get_model_mat(sphere);
    Mat4 view = cam_get_view_mat(camera);
    Mat4 proj = cam_get_perspective_projection_mat(camera);
    
    glUniformMatrix4fv(sphere->u_model, 1, GL_TRUE, model.data);
    glUniformMatrix4fv(sphere->u_view, 1, GL_TRUE, view.data);
    glUniformMatrix4fv(sphere->u_proj, 1, GL_TRUE, proj.data);

    glUniform1f(sphere->u_tess_lvl_inner, tess_lvl_inner);
    glUniform1f(sphere->u_tess_lvl_outer, tess_lvl_outer);

    glUniform1i(sphere->u_surface_noise_n_levels, surface_noise_n_levels);
    glUniform1f(sphere->u_surface_noise_freq_mult, surface_noise_freq_mult);
    glUniform1f(sphere->u_surface_noise_ampl_mult, surface_noise_ampl_mult);
    glUniform1f(sphere->u_surface_noise_freq_init, surface_noise_freq_init);
    glUniform1f(sphere->u_surface_noise_mult, surface_noise_mult);

    glUniform3fv(sphere->u_view_pos, 1, camera->translation.data);
    glUniform3fv(sphere->u_light_pos, 1, light_pos->data);
    glUniform3fv(sphere->u_diffuse_color, 1, diffuse_color->data);
    glUniform3fv(sphere->u_light_color, 1, light_color->data);
    glUniform1f(sphere->u_ambient_weight, ambient_weight);
    glUniform1f(sphere->u_specular_power, specular_power);

    glUniform1i(sphere->u_with_shadows, with_shadows);
    glUniform1f(sphere->u_far, camera->far);

    glBindVertexArray(sphere->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->ebo);
    glDrawElements(GL_PATCHES, sizeof(ICOSAHEDRON_FACES) / sizeof(ICOSAHEDRON_FACES[0]),  GL_UNSIGNED_BYTE, 0);
}

bool sphere_create(Sphere* sphere) {
    memset(sphere, 0, sizeof(*sphere));

    if ( !sphere_shader_link_program(sphere, false) || !sphere_shader_link_program(sphere, true)) {
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

    glEnableVertexAttribArray(sphere->a_pos);
    glVertexAttribPointer(sphere->a_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    sphere->program = program;
    sphere->vbo = vbo;
    sphere->vao = vao;
    sphere->ebo = ebo;
    sphere->rotation = vec3_zeros();
    sphere->translation = vec3_zeros();
    sphere->scale = vec3_ones();

    return true;
}

bool sphere_shader_link_program(Sphere* sphere, GLuint program, bool with_shadows) {
    GLuint program = glCreateProgram();

    const char* deps[1] = {"./shaders/common/random.glsl"};
    const char* vert = "./shaders/sphere/sphere.vert"; 
    const char* tesc = "./shaders/sphere/sphere.tesc"; 
    const char* tese = "./shaders/sphere/sphere.tese"; 
    const char* frag = "./shaders/common/lighting.frag"; 
    const char* geom = with_shadows ? "./shaders/common/cube_shadows.geom" : NULL;
    if (!shader_link_program(vert, tesc, tese, geom, frag, program, 1, deps)) {
        return false;
    } else if (with_shadows) {
        bool ok = true;
        ok &= shader_get_uniform_location(&(sphere->u_shadow_transforms[0]), program, "u_shadow_transforms[0]");
        ok &= shader_get_uniform_location(&(sphere->u_shadow_transforms[1]), program, "u_shadow_transforms[1]");
        ok &= shader_get_uniform_location(&(sphere->u_shadow_transforms[2]), program, "u_shadow_transforms[2]");
        ok &= shader_get_uniform_location(&(sphere->u_shadow_transforms[3]), program, "u_shadow_transforms[3]");
        ok &= shader_get_uniform_location(&(sphere->u_shadow_transforms[4]), program, "u_shadow_transforms[4]");
        ok &= shader_get_uniform_location(&(sphere->u_shadow_transforms[5]), program, "u_shadow_transforms[5]");
        if (!ok) {
            return false;
        }
        sphere->shadow_program = program;
    } else {
        sphere->color_program = program;
    }

    if (
        || !shader_get_attrib_location(&(sphere->a_pos), program, "a_pos")
        || !shader_get_uniform_location(&(sphere->u_model), program, "u_model")
        || !shader_get_uniform_location(&(sphere->u_view), program, "u_view")
        || !shader_get_uniform_location(&(sphere->u_proj), program, "u_proj")
        || !shader_get_uniform_location(&(sphere->u_tess_lvl_inner), program, "u_tess_lvl_inner")
        || !shader_get_uniform_location(&(sphere->u_tess_lvl_outer), program, "u_tess_lvl_outer")
        || !shader_get_uniform_location(&(sphere->u_surface_noise_n_levels), program, "u_surface_noise_n_levels")
        || !shader_get_uniform_location(&(sphere->u_surface_noise_freq_mult), program, "u_surface_noise_freq_mult")
        || !shader_get_uniform_location(&(sphere->u_surface_noise_ampl_mult), program, "u_surface_noise_ampl_mult")
        || !shader_get_uniform_location(&(sphere->u_surface_noise_freq_init), program, "u_surface_noise_freq_init")
        || !shader_get_uniform_location(&(sphere->u_surface_noise_mult), program, "u_surface_noise_mult")
        || !shader_get_uniform_location(&(sphere->u_view_pos), program, "u_view_pos")
        || !shader_get_uniform_location(&(sphere->u_light_pos), program, "u_light_pos")
        || !shader_get_uniform_location(&(sphere->u_diffuse_color), program, "u_diffuse_color")
        || !shader_get_uniform_location(&(sphere->u_light_color), program, "u_light_color")
        || !shader_get_uniform_location(&(sphere->u_ambient_weight), program, "u_ambient_weight")
        || !shader_get_uniform_location(&(sphere->u_specular_power), program, "u_specular_power")
        || !shader_get_uniform_location(&(sphere->u_with_shadows), program, "u_with_shadows")
        || !shader_get_uniform_location(&(sphere->u_far), program, "u_far")
    ) {
        return false;
    }

    return true;
}



