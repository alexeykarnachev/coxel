typedef struct Renderer {
    size_t viewport_width;
    size_t viewport_height;
} Renderer;

Renderer RENDERER;
size_t _RENDERER_CREATED = false;

bool renderer_create() {
    bool ok = true;
    ok &= program_create_all();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    _RENDERER_CREATED = ok;
    return ok;
}

bool renderer_update() {
    if (!_RENDERER_CREATED) {
        fprintf(stderr, "ERROR: can't update. Create a renderer first\n");
        return false;
    }

    int32_t camera_lid = scene_get_active_camera_lid();
    if (camera_lid == -1) {
        fprintf(stderr, "ERROR: can't update. Add camera to a scene first\n");
        return false;
    }

    if (
        RENDERER.viewport_width != INPUT.window_width
        || RENDERER.viewport_height != INPUT.window_height
    ) {
        RENDERER.viewport_width = INPUT.window_width;
        RENDERER.viewport_height = INPUT.window_height;
        glViewport(0, 0, INPUT.window_width, INPUT.window_height);
    }

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(PROGRAM_MATERIAL);
    program_set_uniform_1i(PROGRAM_MATERIAL, "camera_id", camera_lid);
    ArrayBuffer* current_mesh_buffer;

    for (size_t i_component = 0; i_component < SCENE_N_COMPONENTS; ++i_component) {
        SceneComponent* component = &SCENE_COMPONENTS[i_component];
        size_t lid = component->lid;
        switch (component->type) {
            case MESH_T:
                program_set_attribute(PROGRAM_MATERIAL, "model_pos", 3, GL_FLOAT);
                current_mesh_buffer = &SCENE_MESH_BUFFERS[lid]; 
                glBindVertexArray(current_mesh_buffer->vao);
                glBindBuffer(GL_ARRAY_BUFFER, current_mesh_buffer->vbo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_mesh_buffer->ebo);
                break;
            case MATERIAL_T:
                program_set_uniform_1i(PROGRAM_MATERIAL, "material_id", lid);
                break;
            case TRANSFORMATION_T:
                program_set_uniform_1i(PROGRAM_MATERIAL, "transformation_id", lid);
                glDrawElements(GL_TRIANGLES, current_mesh_buffer->n_faces, GL_UNSIGNED_INT, 0);
                break;
        }
    }

    return true;
}


// typedef struct Renderer {
//     Mesh* meshes;
//     size_t n_meshes;
// } Renderer;
// 
// bool renderer_create(Renderer* renderer);
// void renderer_set_program(Renderer* renderer, GLuint program);
// bool renderer_draw_material(Renderer* renderer, Mesh* mesh);
// bool renderer_draw_shadows(Renderer* renderer, Mesh* mesh);
// void renderer_set_scene(
//     Renderer* renderer,
//     Mesh* meshes,
//     size_t n_meshes
// );
// void renderer_draw_triangles(Renderer* renderer);
// void renderer_draw_patches(Renderer* renderer, size_t n_vertices_in_patch);
// 
// 
// bool renderer_create(Renderer* renderer) {
//     bool ok = true;
//     ok &= program_create_all();
// 
//     return ok;
// }
// 
// bool renderer_draw_scene(Renderer* renderer, size_t viewport_width, size_t viewport_height) {
//     glEnable(GL_CULL_FACE);
//     glEnable(GL_DEPTH_TEST);
//     glCullFace(GL_BACK);
//     bool ok = true;
// 
//     glBindFramebuffer(GL_FRAMEBUFFER, POINT_SHADOW_CASTER_FBO);
//     glViewport(0, 0, POINT_SHADOW_SIZE, POINT_SHADOW_SIZE);
//     glClearColor(0.0, 0.0, 0.0, 0.0);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//     for (size_t i = 0; i < renderer->n_meshes; ++i) {
//         ok &= renderer_draw_shadows(renderer, &(renderer->meshes[i]));
//     }
// 
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
//     glViewport(0, 0, viewport_width, viewport_height);
//     glClearColor(0.0, 0.0, 0.0, 0.0);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//     glActiveTexture(0);
//     glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, POINT_SHADOW_CASTER_TEX);
//     for (size_t i = 0; i < renderer->n_meshes; ++i) {
//         ok &= renderer_draw_material(renderer, &(renderer->meshes[i]));
//     }
// 
//     if (!ok) {
//         return false;
//     }
// 
//     return true;
// }
// 
// bool renderer_draw_shadows(Renderer* renderer, Mesh* mesh) {
//     GLuint p = PROGRAM_DEPTH_CUBEMAP;
//     glUseProgram(p);
//     mesh_bind(mesh);
// 
//     bool ok = true;
//     ok &= program_set_attribute(p, "model_pos", 3, GL_FLOAT); 
//     ok &= program_set_uniform_matrix_4fv(p, "world_mat", mesh->transformation.world_mat.data, 1, GL_TRUE);
// 
//     if (!ok) {
//         return false;
//     }
// 
//     glDrawElements(GL_TRIANGLES, mesh->n_elements, GL_UNSIGNED_BYTE, 0);
// 
//     return true;
// }
// 
// bool renderer_draw_material(Renderer* renderer, Mesh* mesh) {
//     GLuint p = PROGRAM_MATERIAL;
//     glUseProgram(p);
//     mesh_bind(mesh);
// 
//     bool ok = true;
//     ok &= program_set_attribute(p, "model_pos", 3, GL_FLOAT); 
//     ok &= program_set_uniform_matrix_4fv(p, "world_mat", mesh->transformation.world_mat.data, 1, GL_TRUE);
//     ok &= program_set_uniform_1i(p, "material_id", mesh->material->id);
// 
//     if (!ok) {
//         return false;
//     }
// 
//     glDrawElements(GL_TRIANGLES, mesh->n_elements, GL_UNSIGNED_BYTE, 0);
// 
//     return ok;
// }
// 
// void renderer_set_scene(
//     Renderer* renderer,
//     Mesh* meshes,
//     size_t n_meshes
// ) {
//     renderer->meshes = meshes;
//     renderer->n_meshes = n_meshes;
// }
// 
