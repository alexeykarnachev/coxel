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

    if (SCENE.active_camera_id == -1) {
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
    glBindFramebuffer(GL_FRAMEBUFFER, SCENE.point_shadow_buffer.fbo);
    glViewport(0, 0, POINT_SHADOW_SIZE, POINT_SHADOW_SIZE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _render_point_shadows();

    // Target: gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, SCENE.gbuffer);
    glViewport(0, 0, GBUFFER_WIDTH, GBUFFER_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _render_gbuffer();

    // Target: screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, RENDERER.viewport_width, RENDERER.viewport_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _render_meshes();
    // _render_sun_icon_sprites();
    
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, RENDERER.viewport_width, RENDERER.viewport_height);
    _render_gui_panes();
    _render_gui_texts();

    // glBlitNamedFramebuffer(SCENE.gbuffer, 0, 0, 0, GBUFFER_WIDTH, GBUFFER_HEIGHT, 0, 0, GBUFFER_WIDTH, GBUFFER_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    return true;
}

void _update_scripts() {
    for (size_t script_id = 0; script_id < SCENE.n_scripts; ++script_id) {
        Script* script = &SCENE.scripts[script_id];
        (*script->update)(script->args);
    }
}

void _render_point_shadows() {
    GLuint program = PROGRAM_POINT_SHADOW;
    glUseProgram(program);
    program_set_uniform_1i(program, "camera_id", SCENE.active_camera_id);

    // TODO: factor out this loop
    VAOBuffer* vao_buffer = NULL;
    for (size_t mesh_id = 0; mesh_id < SCENE.n_meshes; ++mesh_id) {
        Mesh* mesh = &SCENE.meshes[mesh_id];
        if (vao_buffer == NULL || mesh->vao_buffer.vao != vao_buffer->vao) {
            vao_buffer = &mesh->vao_buffer;
            vao_buffer_bind(vao_buffer);
            program_set_attribute(program, "model_pos", 3, GL_FLOAT);
        }
        
        Mat4 world_mat = transformation_get_world_mat(&mesh->transformation);
        program_set_uniform_matrix_4fv(program, "world_mat", world_mat.data, 1, true);
        glDrawElements(GL_TRIANGLES, vao_buffer->n_faces, GL_UNSIGNED_INT, 0);
    }
}

void _render_gbuffer() {
    GLuint program = PROGRAM_DEFERRED;
    glUseProgram(program);

    // TODO: factor out this loop
    VAOBuffer* vao_buffer = NULL;
    for (size_t mesh_id = 0; mesh_id < SCENE.n_meshes; ++mesh_id) {
        Mesh* mesh = &SCENE.meshes[mesh_id];
        program_set_uniform_1i(program, "mesh_id", mesh_id);

        if (vao_buffer == NULL || mesh->vao_buffer.vao != vao_buffer->vao) {
            vao_buffer = &mesh->vao_buffer;
            vao_buffer_bind(vao_buffer);
            program_set_attribute(program, "model_pos", 3, GL_FLOAT);
        }
        
        Mat4 world_mat = transformation_get_world_mat(&mesh->transformation);
        program_set_uniform_matrix_4fv(program, "world_mat", world_mat.data, 1, true);
        glDrawElements(GL_TRIANGLES, vao_buffer->n_faces, GL_UNSIGNED_INT, 0);
    }
}

void _render_meshes() {
    GLuint program = PROGRAM_MATERIAL;
    glUseProgram(program);
    program_set_uniform_1i(program, "camera_id", SCENE.active_camera_id);
    glUniform1i(POINT_SHADOW_TEXTURE_LOCATION_IDX, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, SCENE.point_shadow_buffer.tex);

    VAOBuffer* vao_buffer = NULL;
    int32_t material_id = -1;
    for (size_t mesh_id = 0; mesh_id < SCENE.n_meshes; ++mesh_id) {
        Mesh* mesh = &SCENE.meshes[mesh_id];

        if (vao_buffer == NULL || mesh->vao_buffer.vao != vao_buffer->vao) {
            vao_buffer = &mesh->vao_buffer;
            vao_buffer_bind(vao_buffer);
            program_set_attribute(program, "model_pos", 3, GL_FLOAT);
        }
        
        if (material_id == -1 || mesh->material_id != material_id) {
            material_id = mesh->material_id;
            program_set_uniform_1i(program, "material_id", material_id);
        }
        
        Mat4 world_mat = transformation_get_world_mat(&mesh->transformation);
        program_set_uniform_matrix_4fv(program, "world_mat", world_mat.data, 1, true);
        glDrawElements(GL_TRIANGLES, vao_buffer->n_faces, GL_UNSIGNED_INT, 0);
    }
}

void _render_sprites() {
    GLuint program = PROGRAM_SPRITE;
    glUseProgram(program);
    program_set_uniform_1i(program, "camera_id", SCENE.active_camera_id);

    int32_t tex = -1;
    for (size_t sprite_id = 0; sprite_id < SCENE.n_sprites; ++sprite_id) {
        Sprite* sprite = &SCENE.sprites[sprite_id];
        if (tex == -1 || tex != sprite->tex) {
            tex = sprite->tex;
            glUniform1i(SUN_ICON_TEXTURE_LOCATION_IDX, 0);
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, SCENE.sun_icon_texture);
        }
        program_set_uniform_1i(program, "sprite_id", sprite_id);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

void _render_gui_panes() {
    GLuint program = PROGRAM_GUI_PANE; 
    glUseProgram(program);

    for (size_t pane_id = 0; pane_id < SCENE.n_gui_panes; ++pane_id) {
        GUIPane* pane = &SCENE.gui_panes[pane_id];
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
    glBindTexture(GL_TEXTURE_1D, SCENE.gui_font_texture);

    for (size_t text_id = 0; text_id < SCENE.n_gui_texts; ++text_id) {
        GUIText* text = &SCENE.gui_texts[text_id];
        program_set_uniform_1i(program, "screen_height", RENDERER.viewport_height);
        program_set_uniform_1i(program, "gui_text_id", text_id);
        glDrawArrays(GL_TRIANGLES, 0, 6 * text->n_chars);
    }
}

