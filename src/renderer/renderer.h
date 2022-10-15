typedef struct Renderer {
    GLuint program;
    GLuint fbo;
    Mesh* mesh;
} Renderer;


void renderer_create(Renderer* renderer);
void renderer_set_program(Renderer* renderer, GLuint program);
void renderer_set_fbo(Renderer* renderer, GLuint fbo);
bool renderer_set_mesh(Renderer* renderer, Mesh* mesh);
bool renderer_set_scene(Renderer* renderer, Camera* camera, PointLight* point_light);
bool renderer_draw_triangles(Renderer* renderer);
bool renderer_draw_patches(Renderer* renderer, size_t n_vertices_in_patch);


void renderer_create(Renderer* renderer) {
    clear_struct(renderer);

    renderer->fbo = 0;
}

void renderer_set_program(Renderer* renderer, GLuint program) {
    renderer->program = program;
}

void renderer_set_fbo(Renderer* renderer, GLuint fbo) {
    renderer->fbo = fbo;
}

bool renderer_set_mesh(Renderer* renderer, Mesh* mesh) {
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->fbo);

    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

    renderer->mesh = mesh;
    GLuint p = renderer->program;
    return
        program_set_attribute(p, "model_pos", 3, GL_FLOAT)
        && program_set_uniform_3fv(p, "diffuse_color", mesh->material->diffuse_color.data, 1)
        && program_set_uniform_3fv(p, "ambient_color", mesh->material->ambient_color.data, 1)
        && program_set_uniform_3fv(p, "specular_color", mesh->material->specular_color.data, 1)
        && program_set_uniform_1f(p, "shininess", mesh->material->shininess)
        && program_set_uniform_matrix_4fv(p, "world_mat", mesh->transformation->world_mat.data, 1, GL_TRUE);
}

bool renderer_set_scene(Renderer* renderer, Camera* camera, PointLight* point_light) {
    GLuint p = renderer->program;
    return
        program_set_uniform_matrix_4fv(p, "view_mat", camera->view_mat.data, 1, GL_TRUE)
        && program_set_uniform_matrix_4fv(p, "proj_mat", camera->proj_mat.data, 1, GL_TRUE)
        && program_set_uniform_3fv(p, "eye_world_pos", camera->pos.data, 1)
        && program_set_uniform_3fv(p, "point_light_world_pos", point_light->world_pos.data, 1)
        && program_set_uniform_3fv(p, "point_light_color", point_light->color.data, 1)
        && program_set_uniform_1f(p, "point_light_energy", point_light->energy);
}

bool renderer_draw_triangles(Renderer* renderer) {
    glUseProgram(renderer->program);
    glDrawElements(GL_TRIANGLES, renderer->mesh->n_elements, GL_UNSIGNED_BYTE, 0);
}

bool renderer_draw_patches(Renderer* renderer, size_t n_vertices_in_patch) {
    glUseProgram(renderer->program);
    glPatchParameteri(GL_PATCH_VERTICES, n_vertices_in_patch);
    glDrawElements(GL_PATCHES, renderer->mesh->n_elements, GL_UNSIGNED_BYTE, 0);
}

