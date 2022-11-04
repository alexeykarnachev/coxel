typedef struct MeshSelectionArgs {
    int32_t cursor_on_mesh_id;
    int32_t selected_mesh_id;
    Vec3 cursor_on_color;
    Vec3 selected_color;
} MeshSelectionArgs;


Material* _get_material(int32_t mesh_id) {
    if (mesh_id == -1) {
        return NULL;
    }
    size_t material_id = SCENE.meshes[mesh_id].material_id;
    Material* material = &SCENE.materials[material_id]; 
    return material;
}

void _mesh_selection_update(void* args_p) {
    MeshSelectionArgs* args = (MeshSelectionArgs*)(args_p);

    unsigned char id = 0;
    int32_t x = (int32_t)(INPUT.cursor_x * GBUFFER_WIDTH);
    int32_t y = (int32_t)(INPUT.cursor_y * GBUFFER_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, SCENE.gbuffer.fbo);
    glReadPixels(x, y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &id);

    int32_t prev_cursor_on_mesh_id = args->cursor_on_mesh_id;
    int32_t prev_selected_mesh_id = args->selected_mesh_id;
    Material* prev_cursor_on_material = _get_material(prev_cursor_on_mesh_id);
    Material* prev_selected_material = _get_material(prev_selected_mesh_id);

    int32_t curr_cursor_on_mesh_id = (int32_t)id - 1;
    int32_t curr_selected_mesh_id = INPUT.mouse_left_pressed ? curr_cursor_on_mesh_id : args->selected_mesh_id;
    Material* curr_cursor_on_material = _get_material(curr_cursor_on_mesh_id);
    Material* curr_selected_material = _get_material(curr_selected_mesh_id);

    if (prev_cursor_on_material) {
        prev_cursor_on_material->constant_color = vec3_zeros();
        scene_update_mesh(prev_cursor_on_mesh_id);
    }
    if (prev_selected_material) {
        prev_selected_material->constant_color = vec3_zeros();
        scene_update_mesh(prev_selected_mesh_id);
    }
    if (curr_cursor_on_material) {
        curr_cursor_on_material->constant_color = args->cursor_on_color;
        scene_update_mesh(curr_cursor_on_mesh_id);
    }
    if (curr_selected_material) {
        curr_selected_material->constant_color = args->selected_color;
        scene_update_mesh(curr_selected_mesh_id);
    }

    args->selected_mesh_id = curr_selected_mesh_id;
    args->cursor_on_mesh_id = curr_cursor_on_mesh_id;
}

Script mesh_selection_create_script(MeshSelectionArgs* args) {
    Script script;
    script.update = _mesh_selection_update;
    script.args = args;
    return script;
}

