typedef struct Renderer {
    GLuint material_program;
    GLuint shadow_program;
    GLuint current_fbo;

    float shadow_near_plane;
    float shadow_far_plane;
    float shadow_disk_radius;
    float shadow_bias;
    size_t shadow_n_samples;
    DepthCubemap cube_shadowmap;
} Renderer;

bool renderer_create(
    Renderer* renderer,
    size_t shadow_size,
    float shadow_near_plane,
    float shadow_far_plane,
    float shadow_disk_radius,
    float shadow_bias,
    size_t shadow_n_samples
);
void renderer_set_program(Renderer* renderer, GLuint program);
bool renderer_draw_mesh(Renderer* renderer, Mesh* mesh);
bool renderer_set_scene(Renderer* renderer, Camera* camera, PointLight* point_light);
bool renderer_set_depth_cubemap_fbo(Renderer* renderer, DepthCubemap* depth_cubemap);
void renderer_draw_triangles(Renderer* renderer);
void renderer_draw_patches(Renderer* renderer, size_t n_vertices_in_patch);

void _renderer_set_fbo(Renderer* renderer, GLuint fbo, size_t viewport_width, size_t viewport_height);


bool renderer_create(
    Renderer* renderer,
    size_t shadow_size,
    float shadow_near_plane,
    float shadow_far_plane,
    float shadow_disk_radius,
    float shadow_bias,
    size_t shadow_n_samples
) {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    renderer->material_program = glCreateProgram();
    renderer->shadow_program = glCreateProgram();
    program_create_material(renderer->material_program);
    program_create_depth_cubemap(renderer->shadow_program);
    
    renderer->shadow_near_plane = shadow_near_plane;
    renderer->shadow_far_plane = shadow_far_plane;
    renderer->shadow_disk_radius = shadow_disk_radius;
    renderer->shadow_bias = shadow_bias;
    renderer->shadow_n_samples = shadow_n_samples;
    depth_cubemap_create(&renderer->cube_shadowmap, shadow_size);
}

bool renderer_draw_shadows(Renderer* renderer, Mesh* mesh) {
    _renderer_set_fbo(
        renderer,
        renderer->cube_shadowmap.fbo,
        renderer->cube_shadowmap.size, 
        renderer->cube_shadowmap.size
    );

    mesh_bind(mesh);
    glUseProgram(renderer->material_program);

    bool ok = true;
    GLuint p = renderer->shadow_program;
    ok &= program_set_attribute(p, "model_pos", 3, GL_FLOAT); 
    ok &= program_set_uniform_matrix_4fv(p, "world_mat", mesh->transformation.world_mat.data, 1, GL_TRUE);

    glDrawElements(GL_TRIANGLES, mesh->n_elements, GL_UNSIGNED_BYTE, 0);

    return ok;
}

bool renderer_draw_materials(
    Renderer* renderer,
    Mesh* mesh,
    size_t viewport_width,
    size_t viewport_height
) {
    _renderer_set_fbo(renderer, 0, viewport_width, viewport_height);

    mesh_bind(mesh);
    glUseProgram(renderer->material_program);
    
    bool ok = true;
    GLuint p = renderer->material_program;
    ok &= program_set_attribute(p, "model_pos", 3, GL_FLOAT); 
    ok &= program_set_uniform_3fv(p, "diffuse_color", mesh->material.diffuse_color.data, 1);
    ok &= program_set_uniform_3fv(p, "ambient_color", mesh->material.ambient_color.data, 1);
    ok &= program_set_uniform_3fv(p, "specular_color", mesh->material.specular_color.data, 1);
    ok &= program_set_uniform_1f(p, "shininess", mesh->material.shininess);
    ok &= program_set_uniform_matrix_4fv(p, "world_mat", mesh->transformation.world_mat.data, 1, GL_TRUE);

    glDrawElements(GL_TRIANGLES, mesh->n_elements, GL_UNSIGNED_BYTE, 0);

    return ok;
}

bool renderer_set_scene(Renderer* renderer, Camera* camera, PointLight* point_light) {
    depth_cubemap_set_view(
        &renderer->cube_shadowmap,
        point_light->world_pos,
        renderer->shadow_near_plane,
        renderer->shadow_far_plane
    );

    bool ok = true;
    GLuint p = renderer->material_program;
    ok &= program_set_uniform_matrix_4fv(p, "view_mat", camera->view_mat.data, 1, GL_TRUE);
    ok &= program_set_uniform_matrix_4fv(p, "proj_mat", camera->proj_mat.data, 1, GL_TRUE);
    ok &= program_set_uniform_3fv(p, "eye_world_pos", camera->pos.data, 1);
    ok &= program_set_uniform_3fv(p, "point_light_world_pos", point_light->world_pos.data, 1);
    ok &= program_set_uniform_3fv(p, "point_light_color", point_light->color.data, 1);
    ok &= program_set_uniform_1f(p, "point_light_energy", point_light->energy);
    ok &= program_set_uniform_1f(p, "shadow_far_plane", renderer->shadow_far_plane);
    ok &= program_set_uniform_1i(p, "shadow_n_samples", renderer->shadow_n_samples);
    ok &= program_set_uniform_1f(p, "shadow_disk_radius", renderer->shadow_disk_radius);
    ok &= program_set_uniform_1f(p, "shadow_bias", renderer->shadow_bias);

    p = renderer->shadow_program;
    ok &= program_set_uniform_matrix_4fv(p, "view_proj_mats", renderer->cube_shadowmap.view_proj_mats, 6, GL_TRUE);
    ok &= program_set_uniform_3fv(p, "world_pos", renderer->cube_shadowmap.world_pos.data, 1);
    ok &= program_set_uniform_1f(p, "max_dist", renderer->shadow_far_plane);

    return ok;
}

void _renderer_set_fbo(Renderer* renderer, GLuint fbo, size_t viewport_width, size_t viewport_height) {
    if (renderer->current_fbo == fbo) {
        return;
    }
    renderer->current_fbo = fbo;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, viewport_width, viewport_height);
}

