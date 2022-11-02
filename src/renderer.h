typedef struct Renderer {
    size_t viewport_width;
    size_t viewport_height;
} Renderer;


Renderer RENDERER;
size_t _RENDERER_CREATED = false;

void _update_scripts();
void _render_point_shadows();
void _render_deferred();
void _render_meshes();
void _render_gui_panes();
void _render_gui_texts();

bool renderer_create() {
    bool ok = true;
    ok &= program_create_all();

    _RENDERER_CREATED = ok;
    return ok;
}

bool renderer_update() {
    if (!_RENDERER_CREATED) {
        fprintf(stderr, "ERROR: can't update. Create a renderer first\n");
        return false;
    }

    if (SCENE_ACTIVE_CAMERA_ID == -1) {
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

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    _render_point_shadows();
    _render_deferred();
    _render_meshes();

    glDisable(GL_DEPTH_TEST);

    _render_gui_panes();
    _render_gui_texts();

    return true;
}

void _update_scripts() {
    for (size_t script_id = 0; script_id < SCENE_N_SCRIPTS; ++script_id) {
        Script* script = &SCENE_SCRIPTS[script_id];
        (*script->update)(script->args);
    }
}

void _render_point_shadows() {
    glBindFramebuffer(GL_FRAMEBUFFER, SCENE_POINT_SHADOW_BUFFER.fbo);
    glViewport(0, 0, POINT_SHADOW_SIZE, POINT_SHADOW_SIZE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint program = PROGRAM_POINT_SHADOW;
    glUseProgram(program);
    program_set_uniform_1i(program, "camera_id", SCENE_ACTIVE_CAMERA_ID);

    // TODO: factor out this loop
    ArrayBuffer* array_buffer = NULL;
    for (size_t mesh_id = 0; mesh_id < SCENE_N_MESHES; ++mesh_id) {
        Mesh* mesh = &SCENE_MESHES[mesh_id];
        if (array_buffer == NULL || mesh->array_buffer.vao != array_buffer->vao) {
            array_buffer = &mesh->array_buffer;
            array_buffer_bind(array_buffer);
            program_set_attribute(program, "model_pos", 3, GL_FLOAT);
        }
        
        Mat4 world_mat = transformation_get_world_mat(&mesh->transformation);
        program_set_uniform_matrix_4fv(program, "world_mat", world_mat.data, 1, true);
        glDrawElements(GL_TRIANGLES, array_buffer->n_faces, GL_UNSIGNED_INT, 0);
    }
}

void _render_deferred() {
    glBindFramebuffer(GL_FRAMEBUFFER, SCENE_DEFERRED_BUFFER.fbo);
    glViewport(0, 0, DEFERRED_BUFFER_WIDTH, DEFERRED_BUFFER_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint program = PROGRAM_DEFERRED;
    glUseProgram(program);

    // TODO: factor out this loop
    ArrayBuffer* array_buffer = NULL;
    for (size_t mesh_id = 0; mesh_id < SCENE_N_MESHES; ++mesh_id) {
        Mesh* mesh = &SCENE_MESHES[mesh_id];
        if (array_buffer == NULL || mesh->array_buffer.vao != array_buffer->vao) {
            array_buffer = &mesh->array_buffer;
            array_buffer_bind(array_buffer);
            program_set_attribute(program, "model_pos", 3, GL_FLOAT);
        }
        
        Mat4 world_mat = transformation_get_world_mat(&mesh->transformation);
        program_set_uniform_matrix_4fv(program, "world_mat", world_mat.data, 1, true);
        glDrawElements(GL_TRIANGLES, array_buffer->n_faces, GL_UNSIGNED_INT, 0);
    }
}

void _render_meshes() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, RENDERER.viewport_width, RENDERER.viewport_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint program = PROGRAM_MATERIAL;
    glUseProgram(program);
    program_set_uniform_1i(program, "camera_id", SCENE_ACTIVE_CAMERA_ID);
    glActiveTexture(POINT_SHADOW_TEXTURE_LOCATION_IDX);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, SCENE_POINT_SHADOW_BUFFER.tex);
    glActiveTexture(DEFERRED_TEXTURE_LOCATION_IDX);
    glBindTexture(GL_TEXTURE_2D, SCENE_POINT_SHADOW_BUFFER.tex);

    ArrayBuffer* array_buffer = NULL;
    int32_t material_id = -1;
    for (size_t mesh_id = 0; mesh_id < SCENE_N_MESHES; ++mesh_id) {
        Mesh* mesh = &SCENE_MESHES[mesh_id];
        if (array_buffer == NULL || mesh->array_buffer.vao != array_buffer->vao) {
            array_buffer = &mesh->array_buffer;
            array_buffer_bind(array_buffer);
            program_set_attribute(program, "model_pos", 3, GL_FLOAT);
        }
        
        if (material_id == -1 || mesh->material_id != material_id) {
            material_id = mesh->material_id;
            program_set_uniform_1i(program, "material_id", material_id);
        }
        
        Mat4 world_mat = transformation_get_world_mat(&mesh->transformation);
        program_set_uniform_matrix_4fv(program, "world_mat", world_mat.data, 1, true);
        glDrawElements(GL_TRIANGLES, array_buffer->n_faces, GL_UNSIGNED_INT, 0);
    }
}

void _render_gui_panes() {
    GLuint program = PROGRAM_GUI_PANE; 
    glUseProgram(program);

    for (size_t pane_id = 0; pane_id < SCENE_N_GUI_PANES; ++pane_id) {
        GUIPane* pane = &SCENE_GUI_PANES[pane_id];
        Vec4 pane_vec = {{pane->x, pane->y, pane->width, pane->height}};
        program_set_uniform_4fv(program, "gui_pane", pane_vec.data, 1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

void _render_gui_texts() {
    GLuint program = PROGRAM_GUI_TEXT;
    glUseProgram(program);
    glActiveTexture(GUI_FONT_TEXTURE_LOCATION_IDX);
    glBindTexture(GL_TEXTURE_1D, SCENE_GUI_FONT_TEXTURE);

    for (size_t text_id = 0; text_id < SCENE_N_GUI_TEXTS; ++text_id) {
        GUIText* text = &SCENE_GUI_TEXTS[text_id];
        program_set_uniform_1i(program, "screen_height", RENDERER.viewport_height);
        program_set_uniform_1i(program, "gui_text_id", text_id);
        glDrawArrays(GL_TRIANGLES, 0, 6 * text->n_chars);
    }
}


