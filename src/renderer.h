typedef struct Renderer {
    size_t viewport_width;
    size_t viewport_height;
} Renderer;

Renderer RENDERER;
size_t _RENDERER_CREATED = false;

void _render_material_program_component(int32_t gid, ArrayBuffer** current_mesh_buffer);
void _render_point_shadow_program_component(int32_t gid, ArrayBuffer** current_mesh_buffer);

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

    ArrayBuffer* current_mesh_buffer;

    glBindFramebuffer(GL_FRAMEBUFFER, SCENE_POINT_SHADOW_BUFFER.fbo);
    glViewport(0, 0, POINT_SHADOW_SIZE, POINT_SHADOW_SIZE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(PROGRAM_POINT_SHADOW);
    program_set_uniform_1i(PROGRAM_POINT_SHADOW, "camera_id", camera_lid);

    for (size_t gid = 0; gid < SCENE_N_COMPONENTS; ++gid) {
        _render_point_shadow_program_component(gid, &current_mesh_buffer);
    }

    glUseProgram(PROGRAM_MATERIAL);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, RENDERER.viewport_width, RENDERER.viewport_height);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program_set_uniform_1i(PROGRAM_MATERIAL, "camera_id", camera_lid);

    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, SCENE_POINT_SHADOW_BUFFER.tex);

    for (size_t gid = 0; gid < SCENE_N_COMPONENTS; ++gid) {
        _render_material_program_component(gid, &current_mesh_buffer);
    }

    return true;
}

void _render_material_program_component(int32_t gid, ArrayBuffer** current_mesh_buffer) {
    SceneComponent* component = &SCENE_COMPONENTS[gid];
    size_t lid = component->lid;
    switch (component->type) {
        case MESH_T:
            program_set_attribute(PROGRAM_MATERIAL, "model_pos", 3, GL_FLOAT);
            *current_mesh_buffer = &SCENE_MESH_BUFFERS[lid]; 
            glBindVertexArray((*current_mesh_buffer)->vao);
            glBindBuffer(GL_ARRAY_BUFFER, (*current_mesh_buffer)->vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*current_mesh_buffer)->ebo);
            break;
        case MATERIAL_T:
            program_set_uniform_1i(PROGRAM_MATERIAL, "material_id", lid);
            break;
        case TRANSFORMATION_T:
            program_set_uniform_1i(PROGRAM_MATERIAL, "transformation_id", lid);
            glDrawElements(GL_TRIANGLES, (*current_mesh_buffer)->n_faces, GL_UNSIGNED_INT, 0);
            break;
        case SCRIPT_T: ;
            Script* script = (Script*)(component->ptr);
            (*script->update)(script->args);
    }
}

void _render_point_shadow_program_component(int32_t gid, ArrayBuffer** current_mesh_buffer) {
    SceneComponent* component = &SCENE_COMPONENTS[gid];
    size_t lid = component->lid;
    switch (component->type) {
        case MESH_T:
            program_set_attribute(PROGRAM_POINT_SHADOW, "model_pos", 3, GL_FLOAT);
            *current_mesh_buffer = &SCENE_MESH_BUFFERS[lid]; 
            glBindVertexArray((*current_mesh_buffer)->vao);
            glBindBuffer(GL_ARRAY_BUFFER, (*current_mesh_buffer)->vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*current_mesh_buffer)->ebo);
            break;
        case TRANSFORMATION_T:
            program_set_uniform_1i(PROGRAM_POINT_SHADOW, "transformation_id", lid);
            glDrawElements(GL_TRIANGLES, (*current_mesh_buffer)->n_faces, GL_UNSIGNED_INT, 0);
            break;
    }
}

