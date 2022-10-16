typedef struct Renderer {
    GLuint program;
    GLuint fbo;
    Mesh* mesh;

    float point_shadow_max_dist;
} Renderer;

void renderer_create(Renderer* renderer, float point_shadow_max_dist);
void renderer_set_program(Renderer* renderer, GLuint program);
bool renderer_set_mesh(Renderer* renderer, Mesh* mesh);
bool renderer_set_scene(Renderer* renderer, Camera* camera, PointLight* point_light);
void renderer_draw_triangles(Renderer* renderer);
void renderer_draw_patches(Renderer* renderer, size_t n_vertices_in_patch);


void renderer_create(Renderer* renderer, float point_shadow_max_dist) {
    clear_struct(renderer);

    renderer->fbo = 0;
    renderer->point_shadow_max_dist = point_shadow_max_dist;
}

void renderer_set_program(Renderer* renderer, GLuint program) {
    renderer->program = program;
}

bool renderer_set_mesh(Renderer* renderer, Mesh* mesh) {
    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

    renderer->mesh = mesh;
    GLuint p = renderer->program;
    
    bool ok = true;
    ok &= program_set_attribute(p, "model_pos", 3, GL_FLOAT); 
    ok &= program_set_uniform_3fv(p, "diffuse_color", mesh->material->diffuse_color.data, 1);
    ok &= program_set_uniform_3fv(p, "ambient_color", mesh->material->ambient_color.data, 1);
    ok &= program_set_uniform_3fv(p, "specular_color", mesh->material->specular_color.data, 1);
    ok &= program_set_uniform_1f(p, "shininess", mesh->material->shininess);
    ok &= program_set_uniform_matrix_4fv(p, "world_mat", mesh->transformation->world_mat.data, 1, GL_TRUE);

    return ok;
}

bool renderer_set_scene(Renderer* renderer, Camera* camera, PointLight* point_light) {
    GLuint p = renderer->program;

    bool ok = true;
    ok &= program_set_uniform_matrix_4fv(p, "view_mat", camera->view_mat.data, 1, GL_TRUE);
    ok &= program_set_uniform_matrix_4fv(p, "proj_mat", camera->proj_mat.data, 1, GL_TRUE);
    ok &= program_set_uniform_3fv(p, "eye_world_pos", camera->pos.data, 1);
    ok &= program_set_uniform_3fv(p, "point_light_world_pos", point_light->world_pos.data, 1);
    ok &= program_set_uniform_3fv(p, "point_light_color", point_light->color.data, 1);
    ok &= program_set_uniform_1f(p, "point_light_energy", point_light->energy);
    ok &= program_set_uniform_1f(p, "point_shadow_max_dist", renderer->point_shadow_max_dist);

    return ok;
}

bool renderer_set_depth_cubemap(Renderer* renderer, DepthCubemap* depth_cubemap) {
    GLuint p = renderer->program;

    bool ok = true;
    ok &= program_set_uniform_matrix_4fv(p, "view_proj_mats[0]", depth_cubemap->view_proj_mats[0].data, 1, GL_TRUE);
    ok &= program_set_uniform_matrix_4fv(p, "view_proj_mats[1]", depth_cubemap->view_proj_mats[1].data, 1, GL_TRUE);
    ok &= program_set_uniform_matrix_4fv(p, "view_proj_mats[2]", depth_cubemap->view_proj_mats[2].data, 1, GL_TRUE);
    ok &= program_set_uniform_matrix_4fv(p, "view_proj_mats[3]", depth_cubemap->view_proj_mats[3].data, 1, GL_TRUE);
    ok &= program_set_uniform_matrix_4fv(p, "view_proj_mats[4]", depth_cubemap->view_proj_mats[4].data, 1, GL_TRUE);
    ok &= program_set_uniform_matrix_4fv(p, "view_proj_mats[5]", depth_cubemap->view_proj_mats[5].data, 1, GL_TRUE);
    ok &= program_set_uniform_3fv(p, "origin_world_pos", depth_cubemap->origin_world_pos.data, 1);
    ok &= program_set_uniform_1f(p, "max_dist", depth_cubemap->max_dist);

    return ok;
}

void renderer_draw_triangles(Renderer* renderer) {
    glUseProgram(renderer->program);
    glDrawElements(GL_TRIANGLES, renderer->mesh->n_elements, GL_UNSIGNED_BYTE, 0);
}

void renderer_draw_patches(Renderer* renderer, size_t n_vertices_in_patch) {
    glUseProgram(renderer->program);
    glPatchParameteri(GL_PATCH_VERTICES, n_vertices_in_patch);
    glDrawElements(GL_PATCHES, renderer->mesh->n_elements, GL_UNSIGNED_BYTE, 0);
}

