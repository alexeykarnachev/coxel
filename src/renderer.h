typedef struct Renderer {
    size_t viewport_width;
    size_t viewport_height;
} Renderer;


Renderer RENDERER;
size_t _RENDERER_CREATED = false;

void _update_scripts();
void _render_point_shadows();
void _render_gbuffer();
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
    
    // Target: point shadow buffer
    glBindFramebuffer(GL_FRAMEBUFFER, SCENE_POINT_SHADOW_BUFFER.fbo);
    glViewport(0, 0, POINT_SHADOW_SIZE, POINT_SHADOW_SIZE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _render_point_shadows();

    // Target: gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, SCENE_GBUFFER.fbo);
    glViewport(0, 0, GBUFFER_WIDTH, GBUFFER_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _render_gbuffer();

    glReadBuffer(GL_COLOR_ATTACHMENT0);

    unsigned char b;
    int x = INPUT.cursor_x * INPUT.window_width;
    int y = INPUT.cursor_y * INPUT.window_height;
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, 1, 1, GL_ALPHA, GL_UNSIGNED_BYTE, &b);
    printf("%i\n", b);

    // Target: screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, RENDERER.viewport_width, RENDERER.viewport_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _render_meshes();
    
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, RENDERER.viewport_width, RENDERER.viewport_height);
    _render_gui_panes();
    _render_gui_texts();

    glBlitNamedFramebuffer(SCENE_GBUFFER.fbo, 0, 0, 0, GBUFFER_WIDTH, GBUFFER_HEIGHT, 0, 0, GBUFFER_WIDTH, GBUFFER_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    return true;
}

void _update_scripts() {
    for (size_t script_id = 0; script_id < SCENE_N_SCRIPTS; ++script_id) {
        Script* script = &SCENE_SCRIPTS[script_id];
        (*script->update)(script->args);
    }
}

void _render_point_shadows() {
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

void _render_gbuffer() {
    GLuint program = PROGRAM_DEFERRED;
    glUseProgram(program);

    // TODO: factor out this loop
    ArrayBuffer* array_buffer = NULL;
    for (size_t mesh_id = 0; mesh_id < SCENE_N_MESHES; ++mesh_id) {
        Mesh* mesh = &SCENE_MESHES[mesh_id];
        program_set_uniform_1i(program, "mesh_id", mesh_id);

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
    GLuint program = PROGRAM_MATERIAL;
    glUseProgram(program);
    program_set_uniform_1i(program, "camera_id", SCENE_ACTIVE_CAMERA_ID);
    glUniform1i(POINT_SHADOW_TEXTURE_LOCATION_IDX, 0);
    glUniform1i(GBUFFER_TEXTURE_LOCATION_IDX, 1);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, SCENE_POINT_SHADOW_BUFFER.tex);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, SCENE_GBUFFER.tex);

    ArrayBuffer* array_buffer = NULL;
    int32_t material_id = -1;
    for (size_t mesh_id = 0; mesh_id < SCENE_N_MESHES; ++mesh_id) {
        Mesh* mesh = &SCENE_MESHES[mesh_id];
        program_set_uniform_1i(program, "mesh_id", mesh_id);
        program_set_uniform_2f(program, "cursor_pos", INPUT.cursor_x, INPUT.cursor_y);

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
    glUniform1i(GUI_FONT_TEXTURE_LOCATION_IDX, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_1D, SCENE_GUI_FONT_TEXTURE);

    for (size_t text_id = 0; text_id < SCENE_N_GUI_TEXTS; ++text_id) {
        GUIText* text = &SCENE_GUI_TEXTS[text_id];
        program_set_uniform_1i(program, "screen_height", RENDERER.viewport_height);
        program_set_uniform_1i(program, "gui_text_id", text_id);
        glDrawArrays(GL_TRIANGLES, 0, 6 * text->n_chars);
    }
}

