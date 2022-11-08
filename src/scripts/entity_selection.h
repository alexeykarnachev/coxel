typedef struct EntitySelectionArgs {
    int cursor_on_entity_id;
    int selected_entity_id;
    Vec3 cursor_on_constant_color;
    Vec3 selected_constant_color;
    
    TextureBuffer* gbuffer;
} EntitySelectionArgs;

Material* _get_material(int entity) {
    if (entity == -1) {
        return NULL;
    }
    Material* material = (Material*)COMPONENTS[MATERIAL_T][entity];
    return material;
}

void _entity_selection_update(size_t entity, void* args_p) {
    EntitySelectionArgs* args = (EntitySelectionArgs*)(args_p);

    unsigned char id = 0;
    int x = (int)(INPUT.cursor_x * args->gbuffer->width);
    int y = (int)(INPUT.cursor_y * args->gbuffer->height);
    glBindFramebuffer(GL_FRAMEBUFFER, args->gbuffer->fbo);
    glReadPixels(x, y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &id);

    int curr_cursor_on_entity_id = (int)id - 1;
    int curr_selected_entity_id = INPUT.mouse_left_pressed ? curr_cursor_on_entity_id : args->selected_entity_id;
    Material* curr_cursor_on_material = _get_material(curr_cursor_on_entity_id);
    Material* curr_selected_material = _get_material(curr_selected_entity_id);

    int prev_cursor_on_entity_id = args->cursor_on_entity_id;
    int prev_selected_entity_id = args->selected_entity_id;
    Material* prev_cursor_on_material = _get_material(prev_cursor_on_entity_id);
    Material* prev_selected_material = _get_material(prev_selected_entity_id);

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

    args->selected_entity_id = curr_selected_entity_id;
    args->cursor_on_entity_id = curr_cursor_on_entity_id;
}

EntitySelectionArgs entity_selection_create_default_args(
    TextureBuffer* gbuffer
) {
    Vec3 cursor_on_constant_color = {{0.5, 0.5, 0.0}};
    Vec3 selected_constant_color = {{0.75, 0.75, 0.0}};
    EntitySelectionArgs args = {
        -1, -1,
        cursor_on_constant_color,
        selected_constant_color,
        gbuffer
    };
    return args;
}

Script* entity_selection_create_script(
    EntitySelectionArgs* args
) {
    return script_create(_entity_selection_update, args);
}
