typedef struct EntityMouseSelectionArgs {
    int cursor_on_entity;
    int selected_entity;
    Vec3 cursor_on_constant_color;
    Vec3 selected_constant_color;
    
    TextureBuffer* gbuffer;
} EntityMouseSelectionArgs;

Material* _get_material(int entity) {
    if (entity == -1) {
        return NULL;
    }
    Material* material = (Material*)COMPONENTS[MATERIAL_T][entity];
    return material;
}

void _entity_mouse_selection_update(size_t entity, void* args_p) {
    EntityMouseSelectionArgs* args = (EntityMouseSelectionArgs*)(args_p);

    unsigned char id = 0;
    int x = (int)(INPUT.cursor_x * args->gbuffer->width);
    int y = (int)(INPUT.cursor_y * args->gbuffer->height);
    glBindFramebuffer(GL_FRAMEBUFFER, args->gbuffer->fbo);
    glReadPixels(x, y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &id);

    int curr_cursor_on_entity = (int)id - 1;
    int curr_selected_entity = INPUT.mouse_left_pressed ? curr_cursor_on_entity : args->selected_entity;
    Material* curr_cursor_on_material = _get_material(curr_cursor_on_entity);
    Material* curr_selected_material = _get_material(curr_selected_entity);

    int prev_cursor_on_entity = args->cursor_on_entity;
    int prev_selected_entity = args->selected_entity;
    Material* prev_cursor_on_material = _get_material(prev_cursor_on_entity);
    Material* prev_selected_material = _get_material(prev_selected_entity);

    if (prev_cursor_on_material) {
        prev_cursor_on_material->constant_color = vec3_zeros();
    }
    if (prev_selected_material) {
        prev_selected_material->constant_color = vec3_zeros();
    }
    if (curr_cursor_on_material) {
        curr_cursor_on_material->constant_color = args->cursor_on_constant_color;
    }
    if (curr_selected_material) {
        curr_selected_material->constant_color = args->selected_constant_color;
    }

    args->selected_entity = curr_selected_entity;
    args->cursor_on_entity = curr_cursor_on_entity;
}

EntityMouseSelectionArgs entity_mouse_selection_create_default_args(
    TextureBuffer* gbuffer
) {
    Vec3 cursor_on_constant_color = {{0.5, 0.5, 0.0}};
    Vec3 selected_constant_color = {{0.75, 0.75, 0.0}};
    EntityMouseSelectionArgs args = {
        -1, -1,
        cursor_on_constant_color,
        selected_constant_color,
        gbuffer
    };
    return args;
}

Script* entity_mouse_selection_create_script(
    EntityMouseSelectionArgs* args
) {
    return script_create(_entity_mouse_selection_update, args);
}
