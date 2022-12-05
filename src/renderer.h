typedef struct Renderer {
    size_t viewport_width;
    size_t viewport_height;

    GBuffer gbuffer;
    OverlayBuffer overlay_buffer;
    Texture gui_font_texture;
} Renderer;


void _update_scripts();
void _render_color(
    GBuffer* gbuffer,
    OverlayBuffer* overlay_buffer
);
void _render_gbuffer();
void _render_overlay_buffer(
    size_t viewport_width,
    size_t viewport_height
);
void _render_gui_rects(
    GLuint program,
    size_t viewport_width,
    size_t viewport_height
);
void _render_gui_texts(
    GLuint font_tex, size_t viewport_width, size_t viewport_height);

void _set_uniform_camera(GLuint program);
void _set_uniform_point_lights(GLuint program);
void _set_uniform_material(GLuint program, Material* material);

int renderer_create(
    Renderer* renderer,
    size_t gbuffer_width,
    size_t gbuffer_height,
    size_t overlay_buffer_width,
    size_t overlay_buffer_height
) {
    clear_struct(renderer);

    int ok = 1;
    ok &= program_create_all();

    ok &= gbuffer_create(
        &renderer->gbuffer, gbuffer_width, gbuffer_height);
    ok &= overlay_buffer_create(
        &renderer->overlay_buffer,
        overlay_buffer_width,
        overlay_buffer_height
    );

    ok &= texture_create_1d(
        &renderer->gui_font_texture,
        (void*)GUI_FONT_RASTER,
        0,
        GUI_FONT_TEXTURE_WIDTH,
        GL_R8,
        GL_RED,
        GL_UNSIGNED_BYTE
    );

    return ok;
}

int renderer_update(Renderer* renderer) {
    if (
        renderer->viewport_width != INPUT.window_width
        || renderer->viewport_height != INPUT.window_height
    ) {
        renderer->viewport_width = INPUT.window_width;
        renderer->viewport_height = INPUT.window_height;
        glViewport(0, 0, INPUT.window_width, INPUT.window_height);
    }

    _update_scripts();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    // Target: gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->gbuffer.fbo);
    glViewport(0, 0, renderer->gbuffer.width, renderer->gbuffer.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _render_gbuffer();

    // Terget: overlay buffer
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->overlay_buffer.fbo);
    glViewport(
        0, 0,
        renderer->overlay_buffer.width,
        renderer->overlay_buffer.height
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _render_overlay_buffer(
        renderer->viewport_width,
        renderer->viewport_height
    );
    
    // Target: screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, renderer->viewport_width, renderer->viewport_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _render_color(&renderer->gbuffer, &renderer->overlay_buffer);
    
    return 1;
}

void _update_scripts() {
    for (size_t i = 0; i < N_SCRIPT_ENTITIES; ++i) {
        size_t entity = SCRIPT_ENTITIES[i];
        Script* script = (Script*)COMPONENTS[SCRIPT_T][entity];
        (*script->update)(entity, script->args);
    }
}

void _render_color(GBuffer* gbuffer, OverlayBuffer* overlay_buffer) {
    GLuint program = PROGRAM_COLOR;
    glUseProgram(program);

    _set_uniform_camera(program);
    _set_uniform_point_lights(program);

    // TODO: Factor these out into a cycle or so
    program_set_uniform_1i(program, "world_pos_tex", 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, gbuffer->world_pos_texture.tex);

    program_set_uniform_1i(program, "world_norm_tex", 1);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, gbuffer->world_norm_texture.tex);

    program_set_uniform_1i(program, "diffuse_tex", 2);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, gbuffer->diffuse_texture.tex);

    program_set_uniform_1i(program, "specular_tex", 3);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, gbuffer->specular_texture.tex);

    program_set_uniform_1i(program, "outline_tex", 4);
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, overlay_buffer->outline_texture.tex);

    program_set_uniform_1i(program, "gui_tex", 5);
    glActiveTexture(GL_TEXTURE0 + 5);
    glBindTexture(GL_TEXTURE_2D, overlay_buffer->gui_texture.tex);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void _render_gbuffer() {
    // ----------------------------------------------
    // Meshes:
    GLuint program = PROGRAM_MESH_GBUFFER;
    glUseProgram(program);
    _set_uniform_camera(program);

    VAOBuffer* vao_buffer = NULL;
    for (size_t i = 0; i < N_RENDERABLE_ENTITIES; ++i) {
        size_t entity = RENDERABLE_ENTITIES[i];
        Mat4 world_mat = ecs_get_world_mat(entity);
        Mesh* mesh = (Mesh*)COMPONENTS[MESH_T][entity];

        if (
            vao_buffer == NULL
            || mesh->vao_buffer.vao != vao_buffer->vao
        ) {
            glBindVertexArray(mesh->vao_buffer.vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vao_buffer.ebo);

            glBindBuffer(GL_ARRAY_BUFFER, mesh->vao_buffer.vp_vbo);
            program_set_attribute(program, "model_pos", 3, GL_FLOAT);

            glBindBuffer(GL_ARRAY_BUFFER, mesh->vao_buffer.vn_vbo);
            program_set_attribute(program, "model_norm", 3, GL_FLOAT);
        }

        // TODO: Don't bind the material if already binded.
        Material* material = (Material*)COMPONENTS[MATERIAL_T][entity];
        _set_uniform_material(program, material);

        program_set_uniform_1i(program, "entity_id", entity);
        program_set_uniform_matrix_4fv(
            program, "world_mat", world_mat.data, 1, true);
        glDrawElements(
            GL_TRIANGLES, mesh->vao_buffer.n_f, GL_UNSIGNED_INT, 0);
    }

    // ----------------------------------------------
    // Sprites:
    // TODO: Can I use the sampe program for sprites rendering and for
    // meshes? I can try to treat sprite like a square mesh with a texture.
    program = PROGRAM_SPRITE_GBUFFER;
    glUseProgram(program);
    _set_uniform_camera(program);

    for (size_t i = 0; i < N_SPRITE_ENTITIES; ++i) {
        size_t entity = SPRITE_ENTITIES[i];
        Mat4 world_mat = ecs_get_world_mat(entity);

        // TODO: Don't bind the same texture and the same sprite if
        // already binded.
        Sprite* sprite = (Sprite*)COMPONENTS[SPRITE_T][entity];
        GLuint tex = sprite->texture->tex;
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, tex);

        program_set_uniform_matrix_4fv(
            program, "world_mat", world_mat.data, 1, true);

        float tex_pos[4] = {
            sprite->tex_x,
            sprite->tex_y,
            sprite->tex_width,
            sprite->tex_height};
        program_set_uniform_4fv(program, "tex_pos", tex_pos, 1);
        program_set_uniform_1i(program, "entity_id", entity);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

// TODO: Factor out repetitive code here and in render gbuffer meshes
void _render_overlay_buffer(
    size_t viewport_width,
    size_t viewport_height
) {
    // ----------------------------------------------
    // Mesh outlines:
    GLuint program = PROGRAM_MESH_OUTLINE;
    glUseProgram(program);
    _set_uniform_camera(program);

    VAOBuffer* vao_buffer = NULL;
    for (size_t i = 0; i < N_MESH_WITH_OUTLINE_ENTITIES; ++i) {
        size_t entity = MESH_WITH_OUTLINE_ENTITIES[i];
        Mat4 world_mat = ecs_get_world_mat(entity);
        Mesh* mesh = (Mesh*)COMPONENTS[MESH_T][entity];

        if (
            vao_buffer == NULL
            || mesh->vao_buffer.vao != vao_buffer->vao
        ) {
            glBindVertexArray(mesh->vao_buffer.vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vao_buffer.ebo);

            glBindBuffer(GL_ARRAY_BUFFER, mesh->vao_buffer.vp_vbo);
            program_set_attribute(program, "model_pos", 3, GL_FLOAT);
        }

        program_set_uniform_matrix_4fv(
            program, "world_mat", world_mat.data, 1, true);
        glDrawElements(
            GL_TRIANGLES, mesh->vao_buffer.n_f, GL_UNSIGNED_INT, 0);
    }

    // ----------------------------------------------
    // Sprite outlines:
    program = PROGRAM_SPRITE_OUTLINE;
    glUseProgram(program);
    _set_uniform_camera(program);

    for (size_t i = 0; i < N_SPRITE_WITH_OUTLINE_ENTITIES; ++i) {
        size_t entity = SPRITE_WITH_OUTLINE_ENTITIES[i];
        Mat4 world_mat = ecs_get_world_mat(entity);
        Sprite* sprite = (Sprite*)COMPONENTS[SPRITE_T][entity];

        program_set_uniform_matrix_4fv(
            program, "world_mat", world_mat.data, 1, true);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // ----------------------------------------------
    // GUI rects:
    program = PROGRAM_GUI_RECT;
    glUseProgram(program);

    for (size_t i = 0; i < N_GUI_RECT_ENTITIES; ++i) {
        size_t entity = GUI_RECT_ENTITIES[i];
        GUIRect* rect = (GUIRect*)COMPONENTS[GUI_RECT_T][entity];

        Mat4 world_mat = ecs_get_world_mat(entity);
        program_set_uniform_matrix_4fv(
            program, "world_mat", world_mat.data, 1, true);
        program_set_uniform_1i(program, "width", rect->width);
        program_set_uniform_1i(program, "height", rect->height);
        program_set_uniform_1i(program, "viewport_width", viewport_width);
        program_set_uniform_1i(program, "viewport_height", viewport_height);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

void _render_gui_rects(
    GLuint program,
    size_t viewport_width,
    size_t viewport_height
) {
}

void _render_gui_texts(GLuint font_tex, size_t viewport_width, size_t viewport_height) {
    GLuint program = PROGRAM_GUI_TEXT;
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_1D, font_tex);

    for (size_t i = 0; i < N_GUI_TEXT_ENTITIES; ++i) {
        size_t entity = GUI_TEXT_ENTITIES[i];
        Mat4 world_mat = ecs_get_world_mat(entity);
        GUIText* text = (GUIText*)COMPONENTS[GUI_TEXT_T][entity];

        program_set_uniform_matrix_4fv(
            program, "world_mat", world_mat.data, 1, true);
        program_set_uniform_1i(program, "font_height", text->font_height);
        program_set_uniform_1i(program, "viewport_width", viewport_width);
        program_set_uniform_1i(program, "viewport_height", viewport_height);
        program_set_uniform_1uiv(program, "char_inds", text->char_inds, text->n_chars);
        glDrawArrays(GL_TRIANGLES, 0, 6 * text->n_chars);
    }
}

void _set_uniform_camera(GLuint program) {
    int entity = ecs_get_active_camera_entity();
    if (entity == -1) {
        return;
    }

    Camera* camera = (Camera*)COMPONENTS[CAMERA_T][entity];
    Transformation* transformation =
        (Transformation*)COMPONENTS[TRANSFORMATION_T][entity];

    program_set_uniform_4fv(
        program, "camera.world_pos",
        transformation->translation.data, 1);
    program_set_uniform_matrix_4fv(
        program, "camera.view_mat",
        camera_get_view_mat(camera, transformation).data, 1, 1);
    program_set_uniform_matrix_4fv(
        program, "camera.proj_mat",
        camera_get_perspective_projection_mat(camera).data, 1, 1);
}

void _set_uniform_point_lights(GLuint program) {
    for (size_t i = 0; i < N_POINT_LIGHT_ENTITIES; ++i) {
        size_t entity = POINT_LIGHT_ENTITIES[i];
        Transformation* transformation =
            (Transformation*)COMPONENTS[TRANSFORMATION_T][entity];
        PointLight* point_light =
            (PointLight*)COMPONENTS[POINT_LIGHT_T][entity];

        static char uniform_name_buffer[64];
        sprintf(uniform_name_buffer, "point_lights[%ld].world_pos", i);
        program_set_uniform_4fv(
            program, uniform_name_buffer,
            transformation->translation.data, 1);
        sprintf(uniform_name_buffer, "point_lights[%ld].color", i);
        program_set_uniform_3fv(
            program, uniform_name_buffer,
            point_light->color.data, 1);
        sprintf(uniform_name_buffer, "point_lights[%ld].linear", i);
        program_set_uniform_1f(
            program, uniform_name_buffer,
            point_light->linear);
        sprintf(uniform_name_buffer, "point_lights[%ld].quadratic", i);
        program_set_uniform_1f(
            program, uniform_name_buffer,
            point_light->quadratic);
    }
    program_set_uniform_1i(program, "n_point_lights", N_POINT_LIGHT_ENTITIES);
}

void _set_uniform_material(GLuint program, Material* material) {
    program_set_uniform_3fv(
        program, "material.diffuse_color",
        material->diffuse_color.data, 1);
    program_set_uniform_1f(
        program, "material.specular",
        material->specular);
}
