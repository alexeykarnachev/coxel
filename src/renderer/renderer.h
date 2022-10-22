typedef struct Renderer {
    float shadow_near_plane;
    float shadow_far_plane;
    float shadow_disk_radius;
    float shadow_bias_min;
    float shadow_bias_max;
    size_t shadow_min_n_samples;
    size_t shadow_max_n_samples;
    DepthCubemap cube_shadowmap;

    Camera* camera;

    Mesh* meshes;
    size_t n_meshes;
} Renderer;

bool renderer_create(
    Renderer* renderer,
    size_t shadow_size,
    float shadow_near_plane,
    float shadow_far_plane,
    float shadow_disk_radius,
    float shadow_bias_min,
    float shadow_bias_max,
    size_t shadow_min_n_samples,
    size_t shadow_max_n_samples
);
void renderer_set_program(Renderer* renderer, GLuint program);
bool renderer_draw_material(Renderer* renderer, Mesh* mesh);
bool renderer_draw_shadows(Renderer* renderer, Mesh* mesh);
void renderer_set_scene(
    Renderer* renderer,
    Camera* camera,
    Mesh* meshes,
    size_t n_meshes
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
    float shadow_bias_min,
    float shadow_bias_max,
    size_t shadow_min_n_samples,
    size_t shadow_max_n_samples
) {
    renderer->shadow_near_plane = shadow_near_plane;
    renderer->shadow_far_plane = shadow_far_plane;
    renderer->shadow_disk_radius = shadow_disk_radius;
    renderer->shadow_bias_min = shadow_bias_min;
    renderer->shadow_bias_max = shadow_bias_max;
    renderer->shadow_min_n_samples = shadow_min_n_samples;
    renderer->shadow_max_n_samples = shadow_max_n_samples;

    bool ok = true;
    ok &= program_create_all();
    ok &= depth_cubemap_create(&renderer->cube_shadowmap, shadow_size); 

    return ok;
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
        // ok &= renderer_draw_shadows(renderer, &(renderer->meshes[i]));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewport_width, viewport_height);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, renderer->cube_shadowmap.tex);
    for (size_t i = 0; i < renderer->n_meshes; ++i) {
        ok &= renderer_draw_material(renderer, &(renderer->meshes[i]));
    }

    if (!ok) {
        return false;
    }

    return true;
}

// bool renderer_draw_shadows(Renderer* renderer, Mesh* mesh) {
//     GLuint p = PROGRAM_DEPTH_CUBEMAP;
//     glUseProgram(p);
//     mesh_bind(mesh);
// 
//     static Vec3 world_pos[MAX_N_POINT_SHADOWS];
// 
//     for (size_t i = 0; i < n_point_shadows; ++i) {
//         world_pos[i] = point_lights[i].world_pos;
//     }
// 
//     depth_cubemap_set_views(
//         &renderer->cube_shadowmap,
//         world_pos,
//         n_point_shadows,
//         renderer->shadow_near_plane,
//         renderer->shadow_far_plane
//     );
// 
//     bool ok = true;
//     ok &= program_set_attribute(p, "model_pos", 3, GL_FLOAT); 
//     ok &= program_set_uniform_matrix_4fv(p, "world_mat", mesh->transformation.world_mat.data, 1, GL_TRUE);
// 
//     ok &= program_set_uniform_1f(p, "max_dist", renderer->shadow_far_plane);
//     ok &= program_set_uniform_3fv(p, "world_pos", renderer->cube_shadowmap.world_pos, n_point_shadows);
//     ok &= program_set_uniform_matrix_4fv(
//         p, "view_proj_mats", renderer->cube_shadowmap.view_proj_mats, 6 * n_point_shadows, GL_TRUE);
//     ok &= program_set_uniform_1i(p, "n_views", n_point_shadows);
// 
//     if (!ok) {
//         return false;
//     }
// 
//     glDrawElements(GL_TRIANGLES, mesh->n_elements, GL_UNSIGNED_BYTE, 0);
// 
//     return true;
// }

bool renderer_draw_material(Renderer* renderer, Mesh* mesh) {
    GLuint p = PROGRAM_MATERIAL;
    glUseProgram(p);
    mesh_bind(mesh);

    bool ok = true;
    ok &= program_set_attribute(p, "model_pos", 3, GL_FLOAT); 
    ok &= program_set_uniform_matrix_4fv(p, "view_mat", renderer->camera->view_mat.data, 1, GL_TRUE);
    ok &= program_set_uniform_matrix_4fv(p, "proj_mat", renderer->camera->proj_mat.data, 1, GL_TRUE);
    ok &= program_set_uniform_matrix_4fv(p, "world_mat", mesh->transformation.world_mat.data, 1, GL_TRUE);
    ok &= program_set_uniform_3fv(p, "eye_world_pos", renderer->camera->pos.data, 1);
    ok &= program_set_uniform_1i(p, "material_id", mesh->material->id);

    ok &= program_set_uniform_1f(p, "shadow_far_plane", renderer->shadow_far_plane);
    ok &= program_set_uniform_1i(p, "shadow_min_n_samples", renderer->shadow_min_n_samples);
    ok &= program_set_uniform_1i(p, "shadow_max_n_samples", renderer->shadow_max_n_samples);
    ok &= program_set_uniform_1f(p, "shadow_disk_radius", renderer->shadow_disk_radius);
    ok &= program_set_uniform_1f(p, "shadow_bias_min", renderer->shadow_bias_min);
    ok &= program_set_uniform_1f(p, "shadow_bias_max", renderer->shadow_bias_max);

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
    size_t n_meshes
) {
    renderer->camera = camera;
    renderer->meshes = meshes;
    renderer->n_meshes = n_meshes;
}

