void renderer_bind_scene(Camera* camera, PointLight* point_light);
void renderer_bind_mesh(Mesh* mesh);
bool renderer_draw_mesh_patches(Mesh* mesh, size_t n_vertices_in_patch);
bool renderer_draw_mesh_triangles(Mesh* mesh);


bool renderer_bind_scene(GLuint program, Camera* camera, PointLight* point_light) {
    return
        program_set_uniform_matrix_4fv(program, "view_mat", camera->view_mat.data, 1, GL_TRUE)
        && program_set_uniform_matrix_4fv(program, "proj_mat", camera->proj_mat.data, 1, GL_TRUE)
        && program_set_uniform_3fv(program, "point_light_world_pos", point_light->world_pos.data, 1)
        && program_set_uniform_3fv(program, "point_light_color", point_light->color.data, 1)
        && program_set_uniform_1f(program, "point_light_energy", point_light->energy)
}

bool renderer_bind_mesh(Mesh* mesh) {
    glUseProgram(mesh->program);
    glBindVertexArray(mesh->buffers.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buffers.ebo);

    return
        program_set_uniform_3fv(program, "diffuse_color", mesh->material->diffuse_color.data, 1)
        && program_set_uniform_3fv(program, "ambient_color", mesh->material->ambient_color.data, 1)
        && program_set_uniform_3fv(program, "specular_color", mesh->material->specular_color.data, 1)
        && program_set_uniform_1f(program, "shininess", mesh->material.shininess)
        && program_set_uniform_matrix_4fv(program, "world_mat", mesh->transformation->world_mat.data, 1, GL_TRUE);
}

bool renderer_draw_mesh_triangles(Mesh* mesh) {
    renderer_bind_mesh(mesh);
    glDrawElements(GL_TRIANGLES, mesh->n_elements, GL_UNSIGNED_BYTE, 0);
}

bool renderer_draw_mesh_patches(Mesh* mesh, size_t n_vertices_in_patch) {
    renderer_bind_mesh(mesh);
    glPatchParameteri(GL_PATCH_VERTICES, n_vertices_in_patch);
    glDrawElements(GL_PATCHES, mesh->n_elements, GL_UNSIGNED_BYTE, 0);
}

