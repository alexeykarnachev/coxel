typedef struct EntityMouseDragArgs {
    int* cursor_on_entity;
} EntityMouseDragArgs;


void _entity_mouse_drag_update(size_t entity, void* args_p) {
    if (!INPUT.mouse_left_pressed) {
        return;
    }

    int camera_entity = ecs_get_active_camera_entity();
    if (camera_entity == -1) {
        printf("1\n");
        return;
    }

    EntityMouseDragArgs* args = (EntityMouseDragArgs*)(args_p);
    
    int cursor_on_entity = *(args->cursor_on_entity);
    if (cursor_on_entity == -1) {
        printf("2\n");
        return;
    }

    Transformation* cursor_on_entity_transformation = 
        (Transformation*)COMPONENTS[TRANSFORMATION_T][cursor_on_entity];

    if (!cursor_on_entity_transformation) {
        printf("3\n");
        printf("cursor_on_entity:%i\n", cursor_on_entity);
        return;
    }

    Camera* camera = (Camera*)COMPONENTS[CAMERA_T][camera_entity];
    Transformation* camera_transformation =
        (Transformation*)COMPONENTS[TRANSFORMATION_T][camera_entity];
    Mat4 proj = camera_get_perspective_projection_mat(camera);
    Mat4 view = camera_get_view_mat(camera, camera_transformation);
    Mat4 view_proj = mat4_mat4_mul(&proj, &view);

    Mat4 world = transformation_get_world_mat(
        cursor_on_entity_transformation);
    Mat4 wvp = mat4_mat4_mul(&view_proj, &world);

    cursor_on_entity_transformation->position.data[0] += 0.1;
}

Script* entity_mouse_drag_create_script(
    EntityMouseDragArgs* args
) {
    return script_create(_entity_mouse_drag_update, args);
}
