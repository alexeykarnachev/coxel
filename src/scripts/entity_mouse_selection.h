typedef struct EntityMouseSelectionArgs {
    int entity;
    Vec3 color;
    TextureBuffer* gbuffer;
} EntityMouseSelectionArgs;

Material* _get_material(int entity) {
    if (entity == -1) {
        return NULL;
    }
    Material* material = (Material*)COMPONENTS[MATERIAL_T][entity];
    return material;
}

void _entity_mouse_selection_update(size_t _, void* args_p) {
    EntityMouseSelectionArgs* args = (EntityMouseSelectionArgs*)(args_p);

    unsigned char id = 0;
    int x = (int)(INPUT.cursor_x * args->gbuffer->width);
    int y = (int)(INPUT.cursor_y * args->gbuffer->height);
    glBindFramebuffer(GL_FRAMEBUFFER, args->gbuffer->fbo);
    glReadPixels(x, y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &id);

    // TODO: Refactor this. Make names more consise.
    // curr_cursor_on_entity, prev_cursor_on_entity, etc are too long.
    int entity = (int)id - 1;
    entity = INPUT.mouse_left_released ? entity : args->entity;
    Material* material = _get_material(entity);

    int prev_entity = args->entity;
    Material* prev_material = _get_material(prev_entity);

    if (prev_material) {
        prev_material->constant_color = vec3_zeros();
    }
    if (material) {
        material->constant_color = args->color;
    }

    args->entity = entity;
}

EntityMouseSelectionArgs entity_mouse_selection_create_default_args(
    TextureBuffer* gbuffer
) {
    Vec3 color = {{0.8, 0.8, -1000.0}};
    EntityMouseSelectionArgs args = {-1, color, gbuffer};
    return args;
}

Script* entity_mouse_selection_create_script(
    EntityMouseSelectionArgs* args
) {
    return script_create(_entity_mouse_selection_update, args);
}
