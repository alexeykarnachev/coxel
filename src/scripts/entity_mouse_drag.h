typedef struct EntityMouseDragArgs {
    int* entity;
    int active;
} EntityMouseDragArgs;


void _entity_mouse_drag_update(size_t _, void* args_p) {
    EntityMouseDragArgs* args = (EntityMouseDragArgs*)(args_p);

    if ((!args->active && !INPUT.g_pressed) || INPUT.mouse_left_pressed) {
        args->active = 0;
        return;
    }
    args->active = 1;

    int camera_entity = ecs_get_active_camera_entity();
    if (camera_entity == -1) {
        return;
    }

    
    int entity = *(args->entity);
    if (entity == -1) {
        return;
    }

    Transformation* entity_transformation = 
        (Transformation*)COMPONENTS[TRANSFORMATION_T][entity];
    Vec3 entity_position = entity_transformation->position;

    if (!entity_transformation) {
        return;
    }

    Camera* camera = (Camera*)COMPONENTS[CAMERA_T][camera_entity];
    Transformation* camera_transformation =
        (Transformation*)COMPONENTS[TRANSFORMATION_T][camera_entity];
    Mat4 proj_mat = camera_get_perspective_projection_mat(camera);
    Mat4 view_mat = camera_get_view_mat(camera, camera_transformation);
    Mat4 vp_mat = mat4_mat4_mul(&proj_mat, &view_mat);

    Mat4 proj_inv = mat4_inverse(&proj_mat);
    Mat4 view_inv = mat4_inverse(&view_mat);
    Mat4 vp_inv = mat4_mat4_mul(&view_inv, &proj_inv);

    Vec4 entity_proj_position = mat4_vec3_mul(&vp_mat, &entity_position);

    float dx = INPUT.cursor_dx * 2.0;
    float dy = INPUT.cursor_dy * 2.0;
    entity_proj_position.data[0] += dx * entity_proj_position.data[3];
    entity_proj_position.data[1] += dy * entity_proj_position.data[3];
    Vec4 entity_new_position = mat4_vec4_mul(&vp_inv, &entity_proj_position);
    entity_transformation->position = vec4_to_vec3(&entity_new_position);
}

Script* entity_mouse_drag_create_script(
    EntityMouseDragArgs* args
) {
    return script_create(_entity_mouse_drag_update, args);
}
