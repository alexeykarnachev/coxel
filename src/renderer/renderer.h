typedef struct Renderer {
    GLuint material_program;
    GLuint shadow_program;

    float shadow_near_plane;
    float shadow_far_plane;
    float shadow_disk_radius;
    float shadow_bias;
    size_t shadow_n_samples;
    DepthCubemap cube_shadowmap;

    Camera* camera;

    Mesh* meshes;
    size_t n_meshes;

    PointLight* point_lights;
    size_t n_point_lights;
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
bool renderer_draw_materials(Renderer* renderer, Mesh* mesh, PointLight point_lights[], size_t n_point_lights);
bool renderer_draw_shadows(Renderer* renderer, Mesh* mesh, PointLight point_lights[], size_t n_point_lights);
void renderer_set_scene(
    Renderer* renderer,
    Camera* camera,
    Mesh* meshes,
    size_t n_meshes,
    PointLight* point_lights,
    size_t n_point_lights
);
bool renderer_set_depth_cubemap_fbo(Renderer* renderer, DepthCubemap* depth_cubemap);
void renderer_draw_triangles(Renderer* renderer);
void renderer_draw_patches(Renderer* renderer, size_t n_vertices_in_patch);



bool renderer_create(
    Renderer* renderer,
    size_t shadow_size,
    float shadow_near_plane,
    float shadow_far_plane,
    float shadow_disk_radius,
    float shadow_bias,
    size_t shadow_n_samples
) {
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

bool renderer_draw_scene(Renderer* renderer, size_t viewport_width, size_t viewport_height) {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    bool ok = true;

    glBindFramebuffer(GL_FRAMEBUFFER, renderer->cube_shadowmap.fbo);
    glViewport(0, 0, renderer->cube_shadowmap.size, renderer->cube_shadowmap.size);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (size_t i = 0; i < renderer->n_meshes; ++i) {
        ok &= renderer_draw_shadows(
            renderer, &(renderer->meshes[i]), renderer->point_lights, renderer->n_point_lights);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewport_width, viewport_height);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, renderer->cube_shadowmap.tex);
    for (size_t i = 0; i < renderer->n_meshes; ++i) {
        ok &= renderer_draw_materials(
            renderer, &(renderer->meshes[i]), renderer->point_lights, renderer->n_point_lights);
    }

    if (!ok) {
        return false;
    }

    return true;
}

bool renderer_draw_shadows(Renderer* renderer, Mesh* mesh, PointLight point_lights[], size_t n_point_lights) {
    GLuint p = renderer->shadow_program;
    glUseProgram(p);
    mesh_bind(mesh);

    static Vec3 world_pos[MAX_N_POINT_LIGHTS];
    for (size_t i = 0; i < min(n_point_lights, MAX_N_POINT_LIGHTS); ++i) {
        world_pos[i] = point_lights[i].world_pos;
    }

    depth_cubemap_set_views(
        &renderer->cube_shadowmap,
        world_pos,
        n_point_lights,
        renderer->shadow_near_plane,
        renderer->shadow_far_plane
    );

    bool ok = true;
    ok &= program_set_attribute(p, "model_pos", 3, GL_FLOAT); 
    ok &= program_set_uniform_matrix_4fv(p, "world_mat", mesh->transformation.world_mat.data, 1, GL_TRUE);

    ok &= program_set_uniform_1f(p, "max_dist", renderer->shadow_far_plane);
    ok &= program_set_uniform_3fv(p, "world_pos", renderer->cube_shadowmap.world_pos, n_point_lights);
    ok &= program_set_uniform_matrix_4fv(
        p, "view_proj_mats", renderer->cube_shadowmap.view_proj_mats, 6 * n_point_lights, GL_TRUE);
    ok &= program_set_uniform_1i(p, "n_views", n_point_lights);

    if (!ok) {
        return false;
    }

    glDrawElements(GL_TRIANGLES, mesh->n_elements, GL_UNSIGNED_BYTE, 0);

    return true;
}

bool renderer_draw_materials(Renderer* renderer, Mesh* mesh, PointLight point_lights[], size_t n_point_lights) {
    n_point_lights = min(n_point_lights, MAX_N_POINT_LIGHTS);
    GLuint p = renderer->material_program;
    glUseProgram(p);
    mesh_bind(mesh);

    float point_light_world_pos[3 * MAX_N_POINT_LIGHTS];
    float point_light_color[3 * MAX_N_POINT_LIGHTS];
    float point_light_energy[MAX_N_POINT_LIGHTS];

    for (size_t i = 0; i < n_point_lights; ++i) {
        memcpy(
            &(point_light_world_pos[i * 3]),
            point_lights[i].world_pos.data,
            sizeof(point_lights[i].world_pos.data[0]) * 3
        );
        memcpy(
            &(point_light_color[i * 3]),
            point_lights[i].color.data,
            sizeof(point_lights[i].world_pos.data[0]) * 3
        );
        point_light_energy[i] = point_lights[i].energy;
    }

    bool ok = true;
    ok &= program_set_attribute(p, "model_pos", 3, GL_FLOAT); 
    ok &= program_set_uniform_matrix_4fv(p, "view_mat", renderer->camera->view_mat.data, 1, GL_TRUE);
    ok &= program_set_uniform_matrix_4fv(p, "proj_mat", renderer->camera->proj_mat.data, 1, GL_TRUE);
    ok &= program_set_uniform_matrix_4fv(p, "world_mat", mesh->transformation.world_mat.data, 1, GL_TRUE);
    ok &= program_set_uniform_3fv(p, "eye_world_pos", renderer->camera->pos.data, 1);
    
    ok &= program_set_uniform_3fv(p, "diffuse_color", mesh->material.diffuse_color.data, 1);
    ok &= program_set_uniform_3fv(p, "ambient_color", mesh->material.ambient_color.data, 1);
    ok &= program_set_uniform_3fv(p, "specular_color", mesh->material.specular_color.data, 1);
    ok &= program_set_uniform_1f(p, "shininess", mesh->material.shininess);

    ok &= program_set_uniform_1i(p, "n_point_lights", n_point_lights);
    ok &= program_set_uniform_3fv(p, "point_light_world_pos", point_light_world_pos, n_point_lights);
    ok &= program_set_uniform_3fv(p, "point_light_color", point_light_color, n_point_lights);
    ok &= program_set_uniform_1fv(p, "point_light_energy", point_light_energy, n_point_lights);

    ok &= program_set_uniform_1f(p, "shadow_far_plane", renderer->shadow_far_plane);
    ok &= program_set_uniform_1i(p, "shadow_n_samples", renderer->shadow_n_samples);
    ok &= program_set_uniform_1f(p, "shadow_disk_radius", renderer->shadow_disk_radius);
    ok &= program_set_uniform_1f(p, "shadow_bias", renderer->shadow_bias);

    if (!ok) {
        return false;
    }

    glDrawElements(GL_TRIANGLES, mesh->n_elements, GL_UNSIGNED_BYTE, 0);

    return ok;
}

void renderer_set_scene(
    Renderer* renderer,
    Camera* camera,
    Mesh* meshes,
    size_t n_meshes,
    PointLight* point_lights,
    size_t n_point_lights
) {
    renderer->camera = camera;
    renderer->meshes = meshes;
    renderer->n_meshes = n_meshes;
    renderer->point_lights = point_lights;
    renderer->n_point_lights = n_point_lights;
}

