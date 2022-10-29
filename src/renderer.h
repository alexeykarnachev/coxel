typedef struct Renderer {
    size_t viewport_width;
    size_t viewport_height;
    int32_t camera_lid;
    int32_t gui_font_lid;
} Renderer;

Renderer RENDERER;
size_t _RENDERER_CREATED = false;

void _update_scripts();
void _render_point_shadows();
void _render_materials();
void _render_gui();
ArrayBuffer* _bind_mesh(size_t lid);

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

    RENDERER.camera_lid = scene_get_active_camera_lid();
    if (RENDERER.camera_lid == -1) {
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

    _update_scripts();

    glBindFramebuffer(GL_FRAMEBUFFER, SCENE_POINT_SHADOW_BUFFER.fbo);
    glViewport(0, 0, POINT_SHADOW_SIZE, POINT_SHADOW_SIZE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _render_point_shadows();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, RENDERER.viewport_width, RENDERER.viewport_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _render_materials();
    _render_gui();

    return true;
}

void _update_scripts() {
    for (size_t gid = 0; gid < SCENE_N_COMPONENTS; ++gid) {
        SceneComponent* component = &SCENE_COMPONENTS[gid];
        if (component->type == SCRIPT_T) {
            Script* script = (Script*)(component->ptr);
            (*script->update)(script->args);
        }
    }
}

void _render_point_shadows() {
    glUseProgram(PROGRAM_POINT_SHADOW);
    program_set_uniform_1i(PROGRAM_POINT_SHADOW, "camera_id", RENDERER.camera_lid);

    ArrayBuffer* current_mesh_buffer;
    for (size_t gid = 0; gid < SCENE_N_COMPONENTS; ++gid) {
        SceneComponent* component = &SCENE_COMPONENTS[gid];
        switch (component->type) {
            case MESH_T:
                program_set_attribute(PROGRAM_POINT_SHADOW, "model_pos", 3, GL_FLOAT);
                current_mesh_buffer = _bind_mesh(component->lid);
                break;
            case TRANSFORMATION_T:
                program_set_uniform_1i(PROGRAM_POINT_SHADOW, "transformation_id", component->lid);
                glDrawElements(GL_TRIANGLES, current_mesh_buffer->n_faces, GL_UNSIGNED_INT, 0);
                break;
        }
    }
}

void _render_materials() {
    glUseProgram(PROGRAM_MATERIAL);
    program_set_uniform_1i(PROGRAM_MATERIAL, "camera_id", RENDERER.camera_lid);

    glActiveTexture(POINT_SHADOW_TEXTURE_LOCATION_IDX);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, SCENE_POINT_SHADOW_BUFFER.tex);

    ArrayBuffer* current_mesh_buffer;
    for (size_t gid = 0; gid < SCENE_N_COMPONENTS; ++gid) {
        SceneComponent* component = &SCENE_COMPONENTS[gid];
        switch (component->type) {
            case MESH_T:
                program_set_attribute(PROGRAM_MATERIAL, "model_pos", 3, GL_FLOAT);
                current_mesh_buffer = _bind_mesh(component->lid);
                break;
            case MATERIAL_T:
                program_set_uniform_1i(PROGRAM_MATERIAL, "material_id", component->lid);
                break;
            case TRANSFORMATION_T:
                program_set_uniform_1i(PROGRAM_MATERIAL, "transformation_id", component->lid);
                glDrawElements(GL_TRIANGLES, current_mesh_buffer->n_faces, GL_UNSIGNED_INT, 0);
                break;
        }
    }
}

void _render_gui() {
    glUseProgram(PROGRAM_GUI_PANE);

    glActiveTexture(GUI_FONT_TEXTURE_LOCATION_IDX);
    glBindTexture(GL_TEXTURE_2D, SCENE_GUI_FONT_TEXTURE.tex);
    for (size_t gid = 0; gid < SCENE_N_COMPONENTS; ++gid) {
        SceneComponent* component = &SCENE_COMPONENTS[gid];
        if (component->type == GUI_PANE_T) {
            program_set_uniform_1i(PROGRAM_GUI_PANE, "gui_pane_id", component->lid);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }
}


ArrayBuffer* _bind_mesh(size_t lid) {
    ArrayBuffer* buffer = &SCENE_MESH_BUFFERS[lid]; 
    glBindVertexArray(buffer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ebo);
    return buffer;
}

